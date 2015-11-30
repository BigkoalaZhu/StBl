#include "CorrFinder.h"
#include "Colormaps.h"
#include "QuickMeshDraw.h"
#include <qfile.h>
#include "OBB_Volume.h"

CorrFinder::CorrFinder()
{
	ColorMap = makeColorMapJet();
}


CorrFinder::~CorrFinder()
{
}

void CorrFinder::DrawPartShape()
{
	QuickMeshDraw::drawPartMeshSolid(SourceShape, SurfaceMesh::Vector3(0.5, 0, 0));
	QuickMeshDraw::drawPartMeshSolid(TargetShape, SurfaceMesh::Vector3(-0.5, 0, 0));
}

bool CorrFinder::LoadPairFile(QString filepath, bool hasPart, bool hasInbetween)
{
	QFile pairfile(filepath);
	pairfile_path = filepath;
	if (!pairfile.open(QIODevice::ReadOnly))
		return false;

	QString folder = filepath;
	folder.chop(folder.length() - folder.lastIndexOf("/") - 1);

	QTextStream in(&pairfile);
	QString str = in.readLine();
	QStringList strtmp = str.split(" ");
	sourceShape_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetShape_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	sourceIndex_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetIndex_path = folder + strtmp[1];

	if (hasPart)
	{
		str = in.readLine();
		strtmp = str.split(" ");
		SourceShapePartNum = strtmp[1].toInt();
		SourceShapePartIndex.resize(SourceShapePartNum);
		for (int i = 0; i < SourceShapePartNum; i++)
		{
			str = in.readLine();
			strtmp = str.split(" ");
			for (int j = 0; j < strtmp.size(); j++)
				SourceShapePartIndex[i].push_back(strtmp[j].toInt());
		}

		str = in.readLine();
		strtmp = str.split(" ");
		TargetShapePartNum = strtmp[1].toInt();
		TargetShapePartIndex.resize(TargetShapePartNum);
		for (int i = 0; i < TargetShapePartNum; i++)
		{
			str = in.readLine();
			strtmp = str.split(" ");
			for (int j = 0; j < strtmp.size(); j++)
				TargetShapePartIndex[i].push_back(strtmp[j].toInt());
		}
	}

	if (hasInbetween)
	{
		if (!hasPart)
		{
			str = in.readLine();
			str = in.readLine();
		}
		str = in.readLine();
		strtmp = str.split(" ");
		int InbetweenNum = strtmp[1].toInt();
		for (int i = 0; i < InbetweenNum; i++)
		{
			str = in.readLine();
			InbetweenShapes.push_back(str);
		}
	}
	pairfile.close();

	SourceShape = new SurfaceMeshModel(sourceShape_path);
	SourceShape->read(sourceShape_path.toStdString());
	SourceShape->updateBoundingBox();
	SourceShape->update_face_normals();
	SourceShape->update_vertex_normals();
	SourceShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));
	SourceShape->add_face_property("f:seg", QVector<int>());

	TargetShape = new SurfaceMeshModel(targetShape_path);
	TargetShape->read(targetShape_path.toStdString());
	TargetShape->updateBoundingBox();
	TargetShape->update_face_normals();
	TargetShape->update_vertex_normals();
	TargetShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));
	TargetShape->add_face_property("f:seg", QVector<int>());

	SourceShapeSegmentIndex.resize(SourceShape->faces_size());
	SourceShapePartIndex.resize(SourceShape->faces_size());

	TargetShapeSegmentIndex.resize(TargetShape->faces_size());
	TargetShapePartIndex.resize(TargetShape->faces_size());

	if (!LoadParialPartFile())
		return false;

	ApplySegmentColor();
	return true;
}

void CorrFinder::ApplySegmentColor(int sindex)
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();

	double eachInternal = 1.0f / sourceVaildSegNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < SourceShapeSegmentNum; i++)
				if (SourceShapeSegmentIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (SourceShapeSegmentIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		if (SourceShapeSegmentJointIndex[selected] == -1)
		{
			index++;
			continue;
		}
		fscolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();
	
	eachInternal = 1.0f / targetVaildSegNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < TargetShapeSegmentNum; i++)
				if (TargetShapeSegmentIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (TargetShapeSegmentIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		if (TargetShapeSegmentJointIndex[selected] == -1)
		{
			index++;
			continue;
		}
		ftcolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}
}

void CorrFinder::FindSegAdjacencyMatrix()
{
	Surface_mesh::Face_around_vertex_circulator fit, fend;
	Surface_mesh::Vertex_iterator vit;
	Surface_mesh::Face_property<QVector<int>> fsseg = SourceShape->face_property<QVector<int>>("f:seg");
	Surface_mesh::Face_property<QVector<int>> ftseg = TargetShape->face_property<QVector<int>>("f:seg");

	for (vit = SourceShape->vertices_begin(); vit != SourceShape->vertices_end(); ++vit)
	{
		QVector<int> segs;
		fit = fend = SourceShape->faces(vit);
		do{
			for (int i = 0; i < fsseg[fit].size(); i++)
				segs.push_back(fsseg[fit][i]);
		} while (++fit != fend);
		for (int i = 0; i < segs.size(); i++)
		{
			for (int j = i + 1; j < segs.size(); j++)
			{
				if (segs[i] != segs[j])
				{
					SourceSegAdjacencyMatrix(segs[i], segs[j]) = 1;
					SourceSegAdjacencyMatrix(segs[j], segs[i]) = 1;
				}		
			}
		}
	}

	for (vit = TargetShape->vertices_begin(); vit != TargetShape->vertices_end(); ++vit)
	{
		QVector<int> segs;
		fit = fend = TargetShape->faces(vit);
		do{ 
			for (int i = 0; i < ftseg[fit].size(); i++)
				segs.push_back(ftseg[fit][i]);
		} while (++fit != fend);
		for (int i = 0; i < segs.size(); i++)
		{
			for (int j = i + 1; j < segs.size(); j++)
			{
				if (segs[i] != segs[j])
				{
					TargetSegAdjacencyMatrix(segs[i], segs[j]) = 1;
					TargetSegAdjacencyMatrix(segs[j], segs[i]) = 1;
				}		
			}
		}
	}

}

void CorrFinder::FlatSegMerge(double threshold, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}
	
	Surface_mesh::Vertex_property<Surface_mesh::Vector3> points = proessShape->vertex_property<Surface_mesh::Vector3>("v:point");
	Surface_mesh::Face_iterator fit, fend;
	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	std::vector<std::vector<Vector3d>> SegPoints;
	QVector<int> SegFlat;

	SegFlat.resize(SegNum);
	SegPoints.resize(SegNum);
	fend = proessShape->faces_end();
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		QVector<int> flag;
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1)
				flag.push_back(i);
		fvit = fvend = proessShape->vertices(fit);
		do{
			for (int i = 0; i < flag.size(); i++)
				SegPoints[flag[i]].push_back(points[fvit]);
		} while (++fvit != fvend);
		findex++;
	}
	for (int i = 0; i < SegNum; i++)
	{
		OBB_Volume obb(SegPoints[i]);
		Vector3d extants = obb.extents();

		double maxc = extants.maxCoeff();
		double minc = extants.minCoeff();
		double middlec;
		for (int j = 0; j < 3; j++)
		{
			if (extants[j] != maxc&&extants[j] != minc)
			{
				middlec = extants[j];
				break;
			}
				
		}

		double oth = middlec / minc;
		if (oth > threshold)
			SegFlat[i] = 1;
		else
			SegFlat[i] = 0;
	}
	
	for (int i = 0; i < SegNum; i++)
	{
		for (int j = i + 1; j < SegNum; j++)
		{
			if (SegAdjacencyMatrix(i, j) == 0 || SegFlat[i] * SegFlat[j] == 0)
				continue;
			findex = 0;
			std::vector<Vector3d> tmpPoints;
			for (fit = proessShape->faces_begin(); fit != fend; ++fit){
				if (SegmentIndex[i][findex] == 1 || SegmentIndex[j][findex] == 1)
				{
					fvit = fvend = proessShape->vertices(fit);
					do{
						tmpPoints.push_back(points[fvit]);
					} while (++fvit != fvend);
				}
				findex++;
			}
			OBB_Volume obb(tmpPoints);
			Vector3d extants = obb.extents();

			double maxc = extants.maxCoeff();
			double minc = extants.minCoeff();
			double middlec;
			for (int j = 0; j < 3; j++)
			{
				if (extants[j] != maxc&&extants[j] != minc)
				{
					middlec = extants[j];
					break;
				}

			}
			double oth = middlec / minc;
			if (oth < threshold)
				continue;
			MergeTwoSegs(i, j, SorT);

			SegFlat.erase(SegFlat.begin() + j);
			if (SorT == 0)
				SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
			else
				SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
			j = i + 1;
			SegNum--;
		}
	}

	ApplySegmentColor();
}

void CorrFinder::MergeTwoSegs(int A, int B, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentAxis = SourceShapeSegmentAxis;
		SegmentAxisDirection = SourceShapeSegmentAxisDirection;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentAxis = TargetShapeSegmentAxis;
		SegmentAxisDirection = TargetShapeSegmentAxisDirection;
	}

	int minS = std::min(A, B);
	int maxS = std::max(A, B);

	for (int i = 0; i < proessShape->faces_size(); i++)
	{
		if (SegmentIndex[maxS][i] == 1)
			SegmentIndex[minS][i] = 1;
	}
	SegmentIndex.erase(SegmentIndex.begin() + maxS);

	for (int i = 0; i < SegNum; i++)
	{
		if (SegAdjacencyMatrix(maxS, i) == 1)
		{
			SegAdjacencyMatrix(minS, i) = 1;
			SegAdjacencyMatrix(i, minS) = 1;
		}
	}

	unsigned int numRows = SegAdjacencyMatrix.rows() - 1;
	unsigned int numCols = SegAdjacencyMatrix.cols();

	if (maxS < numRows)
		SegAdjacencyMatrix.block(maxS, 0, numRows - maxS, numCols) = SegAdjacencyMatrix.block(maxS + 1, 0, numRows - maxS, numCols);

	SegAdjacencyMatrix.conservativeResize(numRows, numCols);

	numRows = SegAdjacencyMatrix.rows();
	numCols = SegAdjacencyMatrix.cols() - 1;

	if (maxS < numCols)
		SegAdjacencyMatrix.block(0, maxS, numRows, numCols - maxS) = SegAdjacencyMatrix.block(0, maxS + 1, numRows, numCols - maxS);

	SegAdjacencyMatrix.conservativeResize(numRows, numCols);

	SegNum--;
	SegmentJointIndex.erase(SegmentJointIndex.begin() + maxS);
	SegmentAxis.erase(SegmentAxis.begin() + maxS);
	SegmentAxisDirection.erase(SegmentAxisDirection.begin() + maxS);

	if (SorT == 0)
	{
		SourceShapeSegmentNum = SegNum;
		SourceShapeSegmentIndex = SegmentIndex;
		SourceSegAdjacencyMatrix = SegAdjacencyMatrix;
		SourceShapeSegmentJointIndex = SegmentJointIndex;
		SourceShapeSegmentAxis = SegmentAxis;
		SourceShapeSegmentAxisDirection = SegmentAxisDirection;
	}
	else
	{
		TargetShapeSegmentNum = SegNum;
		TargetShapeSegmentIndex = SegmentIndex;
		TargetSegAdjacencyMatrix = SegAdjacencyMatrix;
		TargetShapeSegmentJointIndex = SegmentJointIndex;
		TargetShapeSegmentAxis = SegmentAxis;
		TargetShapeSegmentAxisDirection = SegmentAxisDirection;
	}
	ApplySeg(SorT);
}

void CorrFinder::GeneratePartSet()
{
	FindSegAdjacencyMatrix();
	FlatSegMerge(1.7, 0);
	FlatSegMerge(1.7, 1);
}

void CorrFinder::ApplyPartColor(int sindex)
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();

	double eachInternal = 1.0f / SourceShapePartNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < SourceShapePartNum; i++)
				if (SourceShapePartIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (SourceShapePartIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		fscolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();

	eachInternal = 1.0f / TargetShapePartNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < TargetShapePartNum; i++)
				if (TargetShapePartIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (TargetShapePartIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		ftcolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}
}

bool CorrFinder::LoadParialPartFile()
{
	QFile sourceparialpart(sourceIndex_path);
	QFile targetparialpart(targetIndex_path);
	if (!sourceparialpart.open(QIODevice::ReadOnly) || !targetparialpart.open(QIODevice::ReadOnly))
		return false;
	QTextStream in_source(&sourceparialpart);
	QTextStream in_target(&targetparialpart);

	QString str;
	QStringList strtmp;

	str = in_source.readLine();
	strtmp = str.split(" ");
	SourceShapeSegmentNum = strtmp[0].toInt();
	sourceVaildSegNum = strtmp[1].toInt();
	SourceShapeSegmentIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxis.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxisDirection.resize(SourceShapeSegmentNum);

	str = in_target.readLine();
	strtmp = str.split(" ");
	TargetShapeSegmentNum = strtmp[0].toInt();
	targetVaildSegNum = strtmp[1].toInt();
	TargetShapeSegmentIndex.resize(TargetShapeSegmentNum);
	TargetShapeSegmentAxis.resize(TargetShapeSegmentNum);
	TargetShapeSegmentAxisDirection.resize(TargetShapeSegmentNum);

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		for (int j = 0; j < 8; j++)
			str = in_source.readLine();
		int axisNum = str.toInt();
		SourceShapeSegmentAxis[i].resize(axisNum);
		SourceShapeSegmentAxisDirection[i].resize(axisNum);

		for (int j = 0; j < axisNum; j++)
		{
			str = in_source.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d vertex(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			SourceShapeSegmentAxis[i][j] = vertex;

			str = in_source.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d direction(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			SourceShapeSegmentAxisDirection[i][j] = vertex;
		}

		str = in_source.readLine();
		int faceNum = str.toInt();
		str = in_source.readLine();
		strtmp = str.split(" ");

		SourceShapeSegmentIndex[i].resize(SourceShape->faces_size());
		SourceShapeSegmentIndex[i].fill(0);
		for (int j = 0; j < faceNum; j++)
		{
			SourceShapeSegmentIndex[i][strtmp[j].toInt()] = 1;
		}
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		for (int j = 0; j < 8; j++)
			str = in_target.readLine();
		int axisNum = str.toInt();
		TargetShapeSegmentAxis[i].resize(axisNum);
		TargetShapeSegmentAxisDirection[i].resize(axisNum);

		for (int j = 0; j < axisNum; j++)
		{
			str = in_target.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d vertex(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			TargetShapeSegmentAxis[i][j] = vertex;

			str = in_target.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d direction(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			TargetShapeSegmentAxisDirection[i][j] = vertex;
		}

		str = in_target.readLine();
		int faceNum = str.toInt();
		str = in_target.readLine();
		strtmp = str.split(" ");

		TargetShapeSegmentIndex[i].resize(TargetShape->faces_size());
		TargetShapeSegmentIndex[i].fill(0);
		for (int j = 0; j < faceNum; j++)
		{
			TargetShapeSegmentIndex[i][strtmp[j].toInt()] = 1;
		}
	}

	str = in_source.readLine();
	str = in_source.readLine();
	str = in_target.readLine();
	str = in_target.readLine();

	while (!in_source.atEnd())
	{
		str = in_source.readLine();
		SourceRealSegIndex.push_back(str.toInt());
	}

	while (!in_target.atEnd())
	{
		str = in_target.readLine();
		TargetRealSegIndex.push_back(str.toInt());
	}

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		if (SourceRealSegIndex[i] == i)
			continue;
		for (int j = 0; j < SourceShapeSegmentIndex[i].size(); j++)
		{
			if (SourceShapeSegmentIndex[i][j])
				SourceShapeSegmentIndex[SourceRealSegIndex[i]][j] = 1;
		}
		SourceRealSegIndex.erase(SourceRealSegIndex.begin()+i);
		SourceShapeSegmentIndex.erase(SourceShapeSegmentIndex.begin()+i);
		SourceShapeSegmentAxis.erase(SourceShapeSegmentAxis.begin()+i);
		SourceShapeSegmentAxisDirection.erase(SourceShapeSegmentAxisDirection.begin() + i);
		i--;
		SourceShapeSegmentNum--;
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		if (TargetRealSegIndex[i] == i)
			continue;
		for (int j = 0; j < TargetShapeSegmentIndex[i].size(); j++)
		{
			if (TargetShapeSegmentIndex[i][j])
				TargetShapeSegmentIndex[TargetRealSegIndex[i]][j] = 1;
		}
		TargetRealSegIndex.erase(TargetRealSegIndex.begin() + i);
		TargetShapeSegmentIndex.erase(TargetShapeSegmentIndex.begin() + i);
		TargetShapeSegmentAxis.erase(TargetShapeSegmentAxis.begin() + i);
		TargetShapeSegmentAxisDirection.erase(TargetShapeSegmentAxisDirection.begin() + i);
		i--;
		TargetShapeSegmentNum--;
	}

	SourceShapeSegmentJointIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < sourceVaildSegNum; i++)
		SourceShapeSegmentJointIndex[i] = 1;

	TargetShapeSegmentJointIndex.resize(TargetShapeSegmentNum);
	TargetShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < targetVaildSegNum; i++)
		TargetShapeSegmentJointIndex[i] = 1;

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

	ApplySeg(0);
	ApplySeg(1);

	return true;
}

void CorrFinder::ApplySeg(int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> RealSegIndex;
	int SegNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		RealSegIndex = SourceRealSegIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		RealSegIndex = TargetRealSegIndex;
	}
	Surface_mesh::Face_iterator fit, fend = proessShape->faces_end();
	Surface_mesh::Face_property<QVector<int>> fseg = proessShape->face_property<QVector<int>>("f:seg");
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit)
	{
		fseg[fit].clear();
		for (int i = 0; i < SegNum; i++)
		{
			if (SegmentIndex[i][findex] == 1)
			{
				fseg[fit].push_back(i);
			}			
		}
		findex++;
	}
}