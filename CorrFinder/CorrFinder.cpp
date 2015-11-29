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
	SourceShape->add_face_property("f:seg", int(-1));

	TargetShape = new SurfaceMeshModel(targetShape_path);
	TargetShape->read(targetShape_path.toStdString());
	TargetShape->updateBoundingBox();
	TargetShape->update_face_normals();
	TargetShape->update_vertex_normals();
	TargetShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));
	TargetShape->add_face_property("f:seg", int(-1));

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
	Surface_mesh::Face_property<int> fsseg = SourceShape->face_property<int>("f:seg");
	Surface_mesh::Face_property<int> ftseg = TargetShape->face_property<int>("f:seg");

	for (vit = SourceShape->vertices_begin(); vit != SourceShape->vertices_end(); ++vit)
	{
		QVector<int> segs;
		fit = fend = SourceShape->faces(vit);
		do{segs.push_back(fsseg[fit]);} while (++fit != fend);
		for (int i = 0; i < segs.size(); i++)
		{
			for (int j = i + 1; j < segs.size(); j++)
			{
				if (segs[i] != segs[j])
					SourceSegAdjacencyMatrix(segs[i], segs[j]) = 1;
			}
		}
	}

}

void CorrFinder::FlatSegMerge(double threshold, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
	}
		
	Surface_mesh::Vertex_property<Surface_mesh::Vector3> points = proessShape->vertex_property<Surface_mesh::Vector3>("v:point");
	Surface_mesh::Face_property<int> fsseg = proessShape->face_property<int>("f:seg");
	Surface_mesh::Face_iterator fit, fend;
	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	std::vector<std::vector<Vector3d>> SegPoints;
	QVector<int> SegFlat;

	SegFlat.resize(SegNum);
	SegPoints.resize(SegNum);
	fend = proessShape->faces_end();
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		int flag = fsseg[fit];
		fvit = fvend = proessShape->vertices(fit);
		do{
			SegPoints[flag].push_back(points[fvit]);
		} while (++fvit != fvend);
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
	int ttt = SegFlat.size();
}

void CorrFinder::GeneratePartSet()
{
	FindSegAdjacencyMatrix();
	FlatSegMerge(3.0, 0);
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
	SourceShapeSegmentJointIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < sourceVaildSegNum; i++)
		SourceShapeSegmentJointIndex[i] = 1;
	SourceShapeSegmentIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxis.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxisDirection.resize(SourceShapeSegmentNum);

	str = in_target.readLine();
	strtmp = str.split(" ");
	TargetShapeSegmentNum = strtmp[0].toInt();
	targetVaildSegNum = strtmp[1].toInt();
	TargetShapeSegmentJointIndex.resize(TargetShapeSegmentNum);
	TargetShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < targetVaildSegNum; i++)
		TargetShapeSegmentJointIndex[i] = 1;
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

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

	return true;
}

void CorrFinder::ApplySeg(int index, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
	}
	Surface_mesh::Face_iterator fit, fend = proessShape->faces_end();
	Surface_mesh::Face_property<int> fseg = proessShape->face_property<int>("f:seg");
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit)
	{
		fseg[fit] = SourceShapeSegmentIndex[index][findex];
		findex++;
	}
}