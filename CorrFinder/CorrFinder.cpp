#include "CorrFinder.h"
#include "Colormaps.h"
#include "QuickMeshDraw.h"
#include <qfile.h>
#include "OBB_Volume.h"
#include "UtilityGlobal.h"
#include "SegGraph.h"

CorrFinder::CorrFinder()
{
	ColorMap = makeColorMapJet();
	ColorDifferent.resize(6);
	ColorDifferent[0] = Qt::GlobalColor::red;
	ColorDifferent[1] = Qt::GlobalColor::green;
	ColorDifferent[2] = Qt::GlobalColor::blue;
	ColorDifferent[3] = Qt::GlobalColor::yellow;
	ColorDifferent[4] = Qt::GlobalColor::cyan;
	ColorDifferent[5] = Qt::GlobalColor::magenta;
}


CorrFinder::~CorrFinder()
{
}

void CorrFinder::DrawPartShape()
{
	QuickMeshDraw::drawPartMeshSolid(SourceShape, SurfaceMesh::Vector3(-0.5, 0, 0));
	QuickMeshDraw::drawPartMeshSolid(TargetShape, SurfaceMesh::Vector3(0.5, 0, 0));
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

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

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

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		QVector<int> segs;
		if (SourceShapeSegmentJointIndex[i] == 1)
			continue;
		for (int j = 0; j < SourceShapeSegmentNum; j++)
		{
			if (i == j || SourceShapeSegmentJointIndex[i] == 1)
				continue;
			if (SourceSegAdjacencyMatrix(i, j) == 1)
				segs.push_back(j);
		}
		for (int k = 0; k < segs.size(); k++)
		{
			for (int j = k + 1; j < segs.size(); j++)
			{
				if (segs[k] != segs[j])
				{
					SourceSegAdjacencyMatrix(segs[k], segs[j]) = 1;
					SourceSegAdjacencyMatrix(segs[j], segs[k]) = 1;
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

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		QVector<int> segs;
		if (TargetShapeSegmentJointIndex[i] == 1)
			continue;
		for (int j = 0; j < TargetShapeSegmentNum; j++)
		{
			if (i == j || TargetShapeSegmentJointIndex[i] == 1)
				continue;
			if (TargetSegAdjacencyMatrix(i, j) == 1)
				segs.push_back(j);
		}
		for (int k = 0; k < segs.size(); k++)
		{
			for (int j = k + 1; j < segs.size(); j++)
			{
				if (segs[k] != segs[j])
				{
					TargetSegAdjacencyMatrix(segs[k], segs[j]) = 1;
					TargetSegAdjacencyMatrix(segs[j], segs[k]) = 1;
				}
			}
		}
	}

}

void CorrFinder::GetSegFaceNum()
{
	SourceSegFaceNum.resize(SourceShapeSegmentNum);
	TargetSegFaceNum.resize(TargetShapeSegmentNum);
	SourceSegFaceNum.fill(0);
	TargetSegFaceNum.fill(0);

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		for each (int flag in SourceShapeSegmentIndex[i])
		{
			SourceSegFaceNum[i] += flag;
		}
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		for each (int flag in TargetShapeSegmentIndex[i])
		{
			TargetSegFaceNum[i] += flag;
		}
	}
}

void CorrFinder::GenerateSegMeshes(int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");
	QVector<SurfaceMeshModel *> SegmentMeshes;
	SegmentMeshes.resize(SegNum);

	for (int i = 0; i < SegNum; i++)
	{
		SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
		piece->reserve(uint(SegFaceNum[i] * 3), uint(SegFaceNum[i] * 6), uint(SegFaceNum[i]));
		for (auto v : proessShape->vertices())
		{
			piece->add_vertex(points[v]);
		}
		SegmentMeshes[i] = piece;
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1)
				SegmentMeshes[i]->add_face(face);
		findex++;
	}

	for (int i = 0; i < SegNum; i++)
	{
		for (auto v : SegmentMeshes[i]->vertices()) if (SegmentMeshes[i]->is_isolated(v)) SegmentMeshes[i]->remove_vertex(v);
		SegmentMeshes[i]->garbage_collection();
		SegmentMeshes[i]->updateBoundingBox();
		SegmentMeshes[i]->update_face_normals();
		SegmentMeshes[i]->update_vertex_normals();
	}
	if (SorT == 0)
		SourceShapeSegment = SegmentMeshes;
	else
		TargetShapeSegment = SegmentMeshes;
}

void CorrFinder::FlatSegMerge(double threshold, int SorT, QVector<int> &flat)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> ShapeSegmentJointIndex;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		ShapeSegmentJointIndex = SourceShapeSegmentJointIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		ShapeSegmentJointIndex = TargetShapeSegmentJointIndex;
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
			if (SegAdjacencyMatrix(i, j) == 0 || SegFlat[i] + SegFlat[j] == 0)
				continue;
/*			findex = 0;
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
			for (int k = 0; k < 3; k++)
			{
				if (extants[k] != maxc&&extants[k] != minc)
				{
					middlec = extants[k];
					break;
				}

			}
			double oth = middlec / minc;
			if (oth < threshold + 0.5)
				continue;*/

			if (!IsFlatMerge(i, j, SorT))
				continue;

			MergeTwoSegs(i, j, SorT);

			SegFlat[i] = 1;
			SegFlat.erase(SegFlat.begin() + j);
			if (SorT == 0)
			{
				SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
				ShapeSegmentJointIndex = SourceShapeSegmentJointIndex;
			}
			else
			{
				SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
				ShapeSegmentJointIndex = TargetShapeSegmentJointIndex;
			}
			j = i + 1;
			SegNum--;
		}
	}

	flat = SegFlat;
	ApplySegmentColor();
}

bool CorrFinder::IsFlatMerge(int indexA, int indexB, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
	}

	Surface_mesh::Vertex_property<Surface_mesh::Vector3> points = proessShape->vertex_property<Surface_mesh::Vector3>("v:point");
	Surface_mesh::Face_iterator fit, fend;
	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	std::vector<std::vector<Vector3d>> SegPoints;

	SegPoints.resize(2);
	fend = proessShape->faces_end();
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		QVector<int> flag;
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1 && (i == indexA || i == indexB))
				flag.push_back(i);
		fvit = fvend = proessShape->vertices(fit);
		do{
			for (int i = 0; i < flag.size(); i++)
			{
				if (flag[i] == indexA)
					SegPoints[0].push_back(points[fvit]);
				else if (flag[i] == indexB)
					SegPoints[1].push_back(points[fvit]);
			}
		} while (++fvit != fvend);
		findex++;
	}

	OBB_Volume obbA(SegPoints[0]);
	OBB_Volume obbB(SegPoints[1]);

	Eigen::Vector3d extantsA = obbA.extents();
	Eigen::Vector3d extantsB = obbB.extents();

	int minA = 0, minB = 0;
	for (int i = 1; i < 3; i++)
	{
		if (extantsA[i] < extantsA[minA])
			minA = i;
		if (extantsB[i] < extantsB[minB])
			minB = i;
	}

	if (abs(extantsA[minA] - extantsB[minB]) / std::min(extantsA[minA], extantsB[minB]) > 1)
		return false;
	
	std::vector<Vector3d> axisA = obbA.axis();
	std::vector<Vector3d> axisB = obbB.axis();

	axisA[minA].normalize();
	axisB[minB].normalize();

	if (abs(axisA[minA].dot(axisB[minB])) < 0.8)
		return false;

	Eigen::Vector3d centerA = obbA.center();
	Eigen::Vector3d centerB = obbB.center();

	Eigen::Vector3d meanAxis;
	
	if (axisA[minA].dot(axisB[minB]) < 0) 
		meanAxis = (axisA[minA] - axisB[minB]) / 2;
	else
		meanAxis = (axisA[minA] + axisB[minB]) / 2;

	centerA = meanAxis.dot(centerA) * meanAxis;
	centerB = meanAxis.dot(centerB) * meanAxis;

	if ((centerA - centerB).norm() > std::max(extantsA[minA], extantsB[minB]) && (centerA + centerB).norm() > std::max(extantsA[minA], extantsB[minB]))
		return false;

	return true;
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

SurfaceMeshModel * CorrFinder::mergedSeg(QVector<int> indexes, int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");

	SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
	int totalFaceNum = 0;
	for each (int n in indexes)
		totalFaceNum += SegFaceNum[n];

	piece->reserve(uint(totalFaceNum * 3), uint(totalFaceNum * 6), uint(totalFaceNum));
	for (auto v : proessShape->vertices())
	{
		piece->add_vertex(points[v]);
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < indexes.size(); i++)
			if (SegmentIndex[indexes[i]][findex] == 1)
			{
				piece->add_face(face);
				break;
			}	
		findex++;
	}

	for (auto v : piece->vertices()) if (piece->is_isolated(v)) piece->remove_vertex(v);
	piece->garbage_collection();
	piece->updateBoundingBox();
	piece->update_face_normals();
	piece->update_vertex_normals();

	return piece;
}

bool CorrFinder::IsFlat(SegmentGroupFromGraph group, int SorT)
{
	for (int i = 0; i < group.labels.size(); i++)
	{
		for (int j = 0; j < group.labels[i].size(); j++)
		{
			if (SorT == 0)
				if (SourceShapeSegmentFlatIndex[group.labels[i][j]] == 1)
					return true;
			if (SorT == 1)
				if (TargetShapeSegmentFlatIndex[group.labels[i][j]] == 1)
					return true;
		}
	}
	return false;
}

void CorrFinder::MergeGraphSegToParts(int SorT)
{
	QVector<SegmentGroupFromGraph> GraphGroups;
	int SegNum;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;

	if (SorT == 0)
	{
		GraphGroups = SourceGraphGroups;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegNum = SourceShapeSegmentNum;
	}
	else
	{
		GraphGroups = TargetGraphGroups;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegNum = TargetShapeSegmentNum;
	}

	int candidatesNum = GraphGroups.size();
	for (int i = 0; i < GraphGroups.size(); i++)
	{
		if (i > candidatesNum)
			candidatesNum = GraphGroups.size();
		if (IsFlat(GraphGroups[i], SorT))
			continue;
		for (int j = 0; j < candidatesNum; j++)
		{
			if (i == j)
				continue;

			if (IsFlat(GraphGroups[j], SorT))
				continue;

			QVector<int> align;
			if (!IsAdjacented(GraphGroups[i], GraphGroups[j], SorT, align))
				continue;

			QVector<int> type;
			if (!IsSmoothConnected(GraphGroups[i], GraphGroups[j], align, type))
				continue;

			SegmentGroupFromGraph newgroup = MergeGroups(GraphGroups[i], GraphGroups[j], type, align);
			if (!IsExistedGroups(GraphGroups, newgroup))
				GraphGroups.push_back(newgroup);
		}
	}

	for (int i = 0; i < GraphGroups.size(); i++)
	{
		GraphGroups[i].joints.resize(GraphGroups[i].labels.size());
		for (int j = 0; j < GraphGroups[i].labels.size(); j++)
		{
			if (GraphGroups[i].labels[j].size() < 2)
				continue;
			for (int k = 0; k < SegNum; k++)
			{
				if (SegmentJointIndex[k] == 1)
					continue;
				int add = 0;
				for (int m = 0; m < GraphGroups[i].labels[j].size(); m++)
					if (SegAdjacencyMatrix(GraphGroups[i].labels[j][m], k) == 1)
					{
						add++;
					}
				if (add > 1)
					GraphGroups[i].joints[j].push_back(k);
			}
		}
	}

	if (SorT == 0)
		SourceGraphGroups = GraphGroups;
	else
		TargetGraphGroups = GraphGroups;
}

bool CorrFinder::IsSmoothConnected(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> align, QVector<int> &type, double threshold)
{
	type.resize(groupA.labels.size());
	for (int i = 0; i < groupA.labels.size(); i++)
	{
		int tmptype;
		if (!IsSmoothConnected(groupA.SegmentAxis[i], groupB.SegmentAxis[align[i]], groupA.SegmentAxisDirection[i], groupB.SegmentAxisDirection[align[i]], tmptype, threshold))
			return false;
		type[i] = tmptype;
	}
	return true;
}

void CorrFinder::MergeSegToParts(int SorT)
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

	QVector<SegmentGroup> SegGroups;
	for (int i = 0; i < SegNum; i++)
	{
		if (SegmentJointIndex[i] == -1)
			continue;
		SegmentGroup newgroup;
		newgroup.labels.push_back(i);
		newgroup.members = mergedSeg(newgroup.labels, SorT);
		newgroup.SegmentAxis = SegmentAxis[i];
		newgroup.SegmentAxisDirection = SegmentAxisDirection[i];
		newgroup.SorT = SorT;
		SegGroups.push_back(newgroup);
	}

	int candidatesNum = SegGroups.size();
	for (int i = 0; i < SegGroups.size(); i++)
	{
		if (i > candidatesNum)
			candidatesNum = SegGroups.size();
		for (int j = 0; j < candidatesNum; j++)
		{
			if (i == j)
				continue;

			if (!IsAdjacented(SegGroups[i], SegGroups[j]))
				continue;

			int type = -1;
			if (!IsSmoothConnected(SegGroups[i], SegGroups[j], type))
				continue;

			SegmentGroup newgroup = MergeGroups(SegGroups[i], SegGroups[j], type);
			if (!IsExistedGroups(SegGroups, newgroup))
				SegGroups.push_back(newgroup);
		}
	}

	for (int i = 0; i < SegGroups.size(); i++)
	{
		if (SegGroups[i].labels.size() < 2)
			continue;
		for (int j = 0; j < SegNum; j++)
		{
			if (SegmentJointIndex[j] == 1)
				continue;
			int add = 0;
			for (int k = 0; k < SegGroups[i].labels.size(); k++)
				if (SegAdjacencyMatrix(SegGroups[i].labels[k], j) == 1)
				{
					add ++;
				}
			if (add > 1)
				SegGroups[i].labels.push_back(j);
		}
	}

	if (SorT == 0)
		SourceSegGroups = SegGroups;
	else
		TargetSegGroups = SegGroups;
}

bool CorrFinder::IsExistedGroups(QVector<SegmentGroup> groups, SegmentGroup test)
{
	for (int i = 0; i < groups.size(); i++)
		if (QVectorisEqual(groups[i].labels, test.labels))
			return true;
	return false;
}

bool CorrFinder::IsExistedGroups(QVector<SegmentGroupFromGraph> groups, SegmentGroupFromGraph test)
{
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].labels.size() != test.labels.size())
			continue;
		for (int j = 0; j < groups[i].labels.size(); j++)
		{
			for (int k = 0; k < groups[i].labels.size(); k++)
			{
				if (groups[i].labels[j].size() != test.labels[k].size())
					continue;
				if (QVectorisEqual(groups[i].labels[j], test.labels[k]))
					return true;
			}
		}
	}
	return false;
}

bool CorrFinder::IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT, QVector<int> &align)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	if (groupA.labels.size() != groupB.labels.size())
		return false;

	Eigen::MatrixXd CorrCandidates = Eigen::MatrixXd::Zero(groupA.labels.size(), groupA.labels.size());
	for (int i = 0; i < groupA.labels.size(); i++)
	{
		for (int j = 0; j < groupB.labels.size(); j++)
		{
			int err = 0;
			if (!IsAdjacented(groupA.labels[i], groupB.labels[j], SorT, err))
			{
				if (err == 1)
					return false;
				continue;
			}
			CorrCandidates(i, j) = 1;
		}
		if (CorrCandidates.row(i).sum() == 0)
			return false;
	}

	QVector<int> test;
	test.resize(groupA.labels.size());
	QVector<QVector<int>> wholeset;
	Permutation(groupA.labels.size(), test, wholeset, 0);

	int flag = 0;
	for (int k = 0; k < wholeset.size(); k++)
	{
		align = wholeset[k];
		flag = 0;
		for (int i = 0; i < groupA.labels.size(); i++)
			if (CorrCandidates(i, align[i]) == 1)
				flag++;
		if (flag == groupA.labels.size())
		{
			break;
		}
	}

	if (flag != groupA.labels.size())
		return false;

	return true;
}

bool CorrFinder::IsAdjacented(QVector<int> indexA, QVector<int> indexB, int SorT, int &err)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	bool flag = false;
	for (int i = 0; i < indexA.size(); i++)
	{
		for (int j = 0; j < indexB.size(); j++)
		{
			if (indexA[i] == indexB[j])
			{
				err = 1;
				return false;
			}
			if (SegAdjacencyMatrix(indexA[i], indexB[j]) == 1)
				flag = true;
		}
	}
	return flag;
}

bool CorrFinder::IsAdjacented(SegmentGroup groupA, SegmentGroup groupB)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (groupA.SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	for (int i = 0; i < groupA.labels.size(); i++)
	{
		for (int j = 0; j < groupB.labels.size(); j++)
		{
			if (groupA.labels[i] == groupB.labels[j])
				return true;
			if (SegAdjacencyMatrix(groupA.labels[i], groupB.labels[j]) == 1)
				return true;
		}
	}
	return false;
}

SegmentGroupFromGraph CorrFinder::MergeGroups(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> type, QVector<int> align)
{
	SegmentGroupFromGraph newgroup;

	newgroup.labels.resize(groupA.labels.size());
	newgroup.SegmentAxis.resize(groupA.labels.size());
	newgroup.SegmentAxisDirection.resize(groupA.labels.size());

	for (int i = 0; i < groupA.labels.size(); i++)
	{
		newgroup.labels[i] = groupA.labels[i];
		for (int j = 0; j < groupB.labels[align[i]].size(); j++)
		{
			newgroup.labels[i].push_back(groupB.labels[align[i]][j]);
		}
		switch (type[i])
		{
		case 0:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_front(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_front(v);
			break;
		case 1:
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			break;
		case 2:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_back(v);
			break;
		case 3:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for (int k = groupB.SegmentAxis[align[i]].size() - 1; k > -1; k--)
				newgroup.SegmentAxis[i].push_back(groupB.SegmentAxis[align[i]][k]);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for (int k = groupB.SegmentAxisDirection[align[i]].size() - 1; k > -1; k--)
				newgroup.SegmentAxisDirection[i].push_back(groupB.SegmentAxisDirection[align[i]][k]);
			break;
		}
	}

	return newgroup;
}

SegmentGroup CorrFinder::MergeGroups(SegmentGroup groupA, SegmentGroup groupB, int type)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;

	if (groupA.SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	SegmentGroup newgroup;
	newgroup.SorT = groupA.SorT;

	newgroup.labels.clear();
	newgroup.SegmentAxis.clear();
	newgroup.SegmentAxisDirection.clear();

	for (int i = 0; i < groupA.labels.size(); i++)
		if (!newgroup.labels.contains(groupA.labels[i]))
			newgroup.labels.push_back(groupA.labels[i]);

	for (int i = 0; i < groupB.labels.size(); i++)
		if (!newgroup.labels.contains(groupB.labels[i]))
			newgroup.labels.push_back(groupB.labels[i]);

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");

	SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
	int totalFaceNum = 0;
	for each (int n in newgroup.labels)
		totalFaceNum += SegFaceNum[n];

	piece->reserve(uint(totalFaceNum * 3), uint(totalFaceNum * 6), uint(totalFaceNum));
	for (auto v : proessShape->vertices())
	{
		piece->add_vertex(points[v]);
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < newgroup.labels.size(); i++)
			if (SegmentIndex[newgroup.labels[i]][findex] == 1)
			{
				piece->add_face(face);
				break;
			}
		findex++;
	}

	for (auto v : piece->vertices()) if (piece->is_isolated(v)) piece->remove_vertex(v);
	piece->garbage_collection();
	piece->updateBoundingBox();
	piece->update_face_normals();
	piece->update_vertex_normals();

	newgroup.members = piece;

	switch (type)
	{
	case 0:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_front(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_front(v);
		break;
	case 1:
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		break;
	case 2:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		break;
	case 3:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for (int i = groupB.SegmentAxis.size() - 1; i > -1; i--)
			newgroup.SegmentAxis.push_back(groupB.SegmentAxis[i]);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for (int i = groupB.SegmentAxisDirection.size() - 1; i > -1; i--)
			newgroup.SegmentAxisDirection.push_back(groupB.SegmentAxisDirection[i]);
		break;
	}

	return newgroup;
}

bool CorrFinder::IsSmoothConnected(QVector<Eigen::Vector3d> PosA, QVector<Eigen::Vector3d> PosB, QVector<Eigen::Vector3d> DirA, QVector<Eigen::Vector3d> DirB, int &type, double threshold)
{
	double distance[4];
	distance[0] = (PosA[0] - PosB[0]).norm();
	distance[1] = (PosA[0] - PosB[PosB.size() - 1]).norm();
	distance[2] = (PosA[PosA.size() - 1] - PosB[0]).norm();
	distance[3] = (PosA[PosA.size() - 1] - PosB[PosB.size() - 1]).norm();

	if (distance[0] <= distance[1] && distance[0] <= distance[2] && distance[0] <= distance[3])
	{
		type = 0;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[0] - PosB[i]).norm() < distance[0])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[i] - PosB[0]).norm() < distance[0])
				return false;
		}

		Eigen::Vector3d d1 = DirA[0].normalized();
		Eigen::Vector3d d2 = DirB[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[1] <= distance[0] && distance[1] <= distance[2] && distance[1] <= distance[3])
	{
		type = 1;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == PosB.size() - 1)
				continue;
			if ((PosA[0] - PosB[i]).norm() < distance[1])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[i] - PosB[PosB.size() - 1]).norm() < distance[1])
				return false;
		}

		Eigen::Vector3d d1 = DirA[0].normalized();
		Eigen::Vector3d d2 = DirB[PosB.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[2] <= distance[0] && distance[2] <= distance[3] && distance[2] <= distance[1])
	{
		type = 2;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[PosA.size() - 1] - PosB[i]).norm() < distance[2])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == PosA.size() - 1)
				continue;
			if ((PosA[i] - PosB[0]).norm() < distance[2])
				return false;
		}

		Eigen::Vector3d d1 = DirA[PosA.size() - 1].normalized();
		Eigen::Vector3d d2 = DirB[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[3] <= distance[0] && distance[3] <= distance[2] && distance[3] <= distance[1])
	{
		type = 3;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == PosB.size() - 1)
				continue;
			if ((PosA[PosA.size() - 1] - PosB[i]).norm() < distance[3])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == PosA.size() - 1)
				continue;
			if ((PosA[i] - PosB[PosB.size() - 1]).norm() < distance[3])
				return false;
		}

		Eigen::Vector3d d1 = DirA[PosA.size() - 1].normalized();
		Eigen::Vector3d d2 = DirB[PosB.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}

	return false;
}

bool CorrFinder::IsSmoothConnected(SegmentGroup groupA, SegmentGroup groupB, int &type, double threshold)
{
	double distance[4];
	distance[0] = (groupA.SegmentAxis[0] - groupB.SegmentAxis[0]).norm();
	distance[1] = (groupA.SegmentAxis[0] - groupB.SegmentAxis[groupB.SegmentAxis.size()-1]).norm();
	distance[2] = (groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[0]).norm();
	distance[3] = (groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm();

	if (distance[0] <= distance[1] && distance[0] <= distance[2] && distance[0] <= distance[3])
	{
		type = 0;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[0] - groupB.SegmentAxis[i]).norm() < distance[0])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[0]).norm() < distance[0])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[0].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}	
	if (distance[1] <= distance[0] && distance[1] <= distance[2] && distance[1] <= distance[3])
	{
		type = 1;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == groupB.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[0] - groupB.SegmentAxis[i]).norm() < distance[1])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm() < distance[1])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[0].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[groupB.SegmentAxis.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[2] <= distance[0] && distance[2] <= distance[3] && distance[2] <= distance[1])
	{
		type = 2;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[i]).norm() < distance[2])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == groupA.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[0]).norm() < distance[2])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[groupA.SegmentAxis.size() - 1].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[3] <= distance[0] && distance[3] <= distance[2] && distance[3] <= distance[1])
	{
		type = 3;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == groupB.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[i]).norm() < distance[3])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == groupA.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm() < distance[3])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[groupA.SegmentAxis.size() - 1].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[groupB.SegmentAxis.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}

	return false;
}

bool CorrFinder::IsFlatMerge(SegmentGroup groupA, SegmentGroup groupB)
{
	OBB_Volume obbA(groupA.members);
	OBB_Volume obbB(groupB.members);

	Eigen::Vector3d extantsA = obbA.extents();
	Eigen::Vector3d extantsB = obbB.extents();

	int minA = 0, minB = 0;
	for (int i = 1; i < 3; i++)
	{
		if (extantsA[i] < extantsA[minA])
			minA = i;
		if (extantsB[i] < extantsB[minB])
			minB = i;
	}

	if (abs(extantsA[minA] - extantsB[minB]) / std::min(extantsA[minA], extantsB[minB]) > 1)
		return false;

	std::vector<Vector3d> axisA = obbA.axis();
	std::vector<Vector3d> axisB = obbB.axis();

	axisA[minA].normalize();
	axisB[minB].normalize();

	if (abs(axisA[minA].dot(axisB[minB])) < 0.8)
		return false;

	Eigen::Vector3d centerA = obbA.center();
	Eigen::Vector3d centerB = obbB.center();

	Eigen::Vector3d meanAxis;

	if (axisA[minA].dot(axisB[minB]) < 0)
		meanAxis = (axisA[minA] - axisB[minB]) / 2;
	else
		meanAxis = (axisA[minA] + axisB[minB]) / 2;

	centerA = meanAxis.dot(centerA) * meanAxis;
	centerB = meanAxis.dot(centerB) * meanAxis;

	if ((centerA - centerB).norm() > std::max(extantsA[minA], extantsB[minB]) && (centerA + centerB).norm() > std::max(extantsA[minA], extantsB[minB]))
		return false;

	return true;
}

void CorrFinder::MergeStraightConnectedCylinders(int SorT)
{
	QVector<SurfaceMeshModel *> ShapeSegment;
	int SegNum;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;

	if (SorT == 0)
	{
		ShapeSegment = SourceShapeSegment;
		SegNum = SourceShapeSegmentNum;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentAxis = SourceShapeSegmentAxis;
		SegmentAxisDirection = SourceShapeSegmentAxisDirection;
	}
	else
	{
		ShapeSegment = TargetShapeSegment;
		SegNum = TargetShapeSegmentNum;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentAxis = TargetShapeSegmentAxis;
		SegmentAxisDirection = TargetShapeSegmentAxisDirection;
	}

	QVector<double> volumes;
	for (int i = 0; i < SegNum; i++)
		volumes.push_back(ShapeSegment[i]->bbox().sizes()[0] * ShapeSegment[i]->bbox().sizes()[1] * ShapeSegment[i]->bbox().sizes()[2]);

	QVector<Eigen::Vector3d > directions;
	for (int i = 0; i < SegNum; i++)
	{
		Eigen::Vector3d tmp = Eigen::Vector3d::Zero();
		for each (Eigen::Vector3d d in SegmentAxisDirection[i])
			tmp += d;
		tmp = tmp / SegmentAxisDirection[i].size();
		directions.push_back(tmp);
	}

	for (int i = 0; i < SegNum; i++)
	{
		for (int j = i + 1; j < SegNum; j++)
		{
			if (SegAdjacencyMatrix(i, j) == 1 && SegmentJointIndex[i] == 1 && SegmentJointIndex[j] == 1)
			{
				double threshold = std::min(volumes[i] / volumes[j], volumes[j] / volumes[i]);
				if (threshold > 0.3)
					continue;
				directions[i].normalize();
				directions[j].normalize();
				threshold = abs(directions[i].dot(directions[j]));
				if (threshold > 0.9)
				{
					MergeTwoSegs(i, j, SorT);
					directions[i] = (directions[i] + directions[j]) / 2;
					volumes[i] = volumes[i] + volumes[j];
					volumes.erase(volumes.begin() + j);
					directions.erase(directions.begin() + j);
					j = i + 1;
					SegNum--;
					if (SorT == 0)
					{
						SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
						SegmentJointIndex = SourceShapeSegmentJointIndex;
					}
					else
					{
						SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
						SegmentJointIndex = TargetShapeSegmentJointIndex;
					}
				}
			}
		}
	}
}

void CorrFinder::GeneratePartSet()
{
	FindSegAdjacencyMatrix();
	FlatSegMerge(2.5, 0, SourceShapeSegmentFlatIndex);
	FlatSegMerge(2.5, 1, TargetShapeSegmentFlatIndex);
	GetSegFaceNum();
	GenerateSegMeshes(0);
	GenerateSegMeshes(1);
	FindSegAdjacencyMatrix();
	GenerateGroupsFromGraph();
	MergeGraphSegToParts(0);
	MergeGraphSegToParts(1);

//	MergeSegToParts(0);
//	MergeSegToParts(1);
}

void CorrFinder::GenerateGroupsFromGraph()
{
	SegGraph GraphS;
	int rnum = 0;
	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		if (SourceShapeSegmentJointIndex[i] == -1)
			continue;
		SegGraphNode tmp;
		tmp.labels.push_back(i);
		tmp.lowest = SourceShapeSegment[i]->bbox().min()[2];
		GraphS.AddNode(tmp);
		rnum++;
	}
	SourceRealSegAdjacencyMatrix = SourceSegAdjacencyMatrix.block(0, 0, rnum, rnum);
	GraphS.AddAdjacencyMatrix(SourceRealSegAdjacencyMatrix);
	GraphS.BuildInitialGraph();
	GraphS.GenerateGroups();

	for (int i = 0; i < GraphS.groups.size(); i++)
	{
		SegmentGroupFromGraph tmp;
		for (int j = 0; j < GraphS.groups[i].size(); j++)
		{
			QVector<int> tmplabel;
			tmplabel.push_back(GraphS.groups[i][j]);
			tmp.labels.push_back(tmplabel);
			tmp.SegmentAxis.push_back(SourceShapeSegmentAxis[GraphS.groups[i][j]]);
			tmp.SegmentAxisDirection.push_back(SourceShapeSegmentAxisDirection[GraphS.groups[i][j]]);
		}
		SourceGraphGroups.push_back(tmp);
	}

	SegGraph GraphT;
	rnum = 0;
	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		if (TargetShapeSegmentJointIndex[i] == -1)
			continue;
		SegGraphNode tmp;
		tmp.labels.push_back(i);
		tmp.lowest = TargetShapeSegment[i]->bbox().min()[2];
		GraphT.AddNode(tmp);
		rnum++;
	}
	TargetRealSegAdjacencyMatrix = TargetSegAdjacencyMatrix.block(0, 0, rnum, rnum);
	GraphT.AddAdjacencyMatrix(TargetRealSegAdjacencyMatrix);
	GraphT.BuildInitialGraph();
	GraphT.GenerateGroups();

	for (int i = 0; i < GraphT.groups.size(); i++)
	{
		SegmentGroupFromGraph tmp;
		for (int j = 0; j < GraphT.groups[i].size(); j++)
		{
			QVector<int> tmplabel;
			tmplabel.push_back(GraphT.groups[i][j]);
			tmp.labels.push_back(tmplabel);
			tmp.SegmentAxis.push_back(TargetShapeSegmentAxis[GraphT.groups[i][j]]);
			tmp.SegmentAxisDirection.push_back(TargetShapeSegmentAxisDirection[GraphT.groups[i][j]]);
		}
		TargetGraphGroups.push_back(tmp);
	}

/*	QVector<QPair<int, int>> Invaild;
	int candidatsNum = SourceGraphGroups.size();
	for (int i = 0; i < SourceGraphGroups.size(); i++)
	{
		if (i > candidatsNum)
			candidatsNum = SourceGraphGroups.size();
		for (int j = 0; j < candidatsNum; j++)
		{
			if (i == j || Invaild.contains(QPair<int, int>(i, j)) || SourceGraphGroups[i].labels.size() != SourceGraphGroups[j].labels.size())
				continue;
			bool flag = false;
			QVector<QVector<int>> Adjacent;
			Adjacent.resize(SourceGraphGroups[i].labels.size());
			int countM = 0, countN = 0;
			for (int m = 0; m < SourceGraphGroups[i].labels.size(); m++)
			{
				bool tmpflag = false;
				for (int n = 0; n < SourceGraphGroups[j].labels.size(); n++)
				{
					if (!IsAdjacented(SourceGraphGroups[i].labels[m], SourceGraphGroups[j].labels[n], 0))
					{
						flag = true;
						m = SourceGraphGroups[i].labels.size();
						break;
					}
					Adjacent[m].push_back(n);
					tmpflag = true;
				}
				if (tmpflag)
					countM++;
			}
			if (flag)
				continue;
			for (int m = 0; m < SourceGraphGroups[j].labels.size(); m++)
			{
				bool tmpflag = false;
				for (int n = 0; n < SourceGraphGroups[i].labels.size(); n++)
				{
					if (!IsAdjacented(SourceGraphGroups[j].labels[m], SourceGraphGroups[i].labels[n], 0))
					{
						flag = true;
						m = SourceGraphGroups[j].labels.size();
						break;
					}
					tmpflag = true;
				}
				if (tmpflag)
					countN++;
			}
			if (flag || countM + countN != 2 * SourceGraphGroups[j].labels.size())
				continue;

			SegmentGroupFromGraph tmp; //Bugs here


		}
	}*/
}

void CorrFinder::DrawSpecificPart(int index, int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<SegmentGroupFromGraph> SegGroups;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegGroups = SourceGraphGroups;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegGroups = TargetGraphGroups;
	}
	Surface_mesh::Face_property<QColor> fcolors = proessShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = proessShape->faces_end();

	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		fcolors[fit] = QColor(255, 255, 255);
		for (int i = 0; i < SegGroups[index].labels.size(); i++)
		{
			bool flag = false;
			for (int j = 0; j < SegGroups[index].labels[i].size(); j++)
			{
				if (SegmentIndex[SegGroups[index].labels[i][j]][findex] == 1)
				{
					fcolors[fit] = ColorDifferent[i % 6];
					flag = true;
					break;
				}
			}

			if (flag)
			{
				i = SegGroups[index].labels.size();
				break;
			}

			for (int j = 0; j < SegGroups[index].joints[i].size(); j++)
			{
				if (SegmentIndex[SegGroups[index].joints[i][j]][findex] == 1)
				{
					fcolors[fit] = ColorDifferent[i % 6];
					flag = true;
					break;
				}
			}

			if (flag)
			{
				i = SegGroups[index].labels.size();
				break;
			}
		}
		findex++;
	}
}

/*void CorrFinder::GenerateInitialGroups(double t)
{
	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		Eigen::AlignedBox3d mbboxi = SourceShapeSegment[i]->bbox();

		SymmetryGroup tmpGroup;
		tmpGroup.members.push_back(SourceShapeSegment[i]);
		tmpGroup.labels.push_back(i);

		for (int j = i + 1; j < SourceShapeSegmentNum; j++)
		{
			double sum = 0;
			Eigen::AlignedBox3d mbboxj = SourceShapeSegment[j]->bbox();
			sum += abs(mbboxi.sizes()[0] - mbboxj.sizes()[0]);
			sum += abs(mbboxi.sizes()[1] - mbboxj.sizes()[1]);
			sum += abs(mbboxi.sizes()[2] - mbboxj.sizes()[2]);

			double threshold = std::min(mbboxi.sizes()[0] + mbboxi.sizes()[1] + mbboxi.sizes()[2], mbboxj.sizes()[0] + mbboxj.sizes()[1] + mbboxj.sizes()[2]);
			if (sum / threshold < t)
			{
				tmpGroup.members.push_back(SourceShapeSegment[j]);
				tmpGroup.labels.push_back(j);
			}
		}

		bool isAdded = false;
		for (int j = 0; j < SourceSegGroups.size(); j++)
		{
			int matched = 0;
			for (int k = 0; k < tmpGroup.labels.size(); k++)
			{
				if (std::find(SourceSegGroups[j].labels.begin(), SourceSegGroups[j].labels.end(), tmpGroup.labels[k]) != SourceSegGroups[j].labels.end())
					matched++;
			}
			if (matched == tmpGroup.labels.size())
			{
				isAdded = true;
				break;
			}
		}
		if (!isAdded)
			SourceSegGroups.push_back(tmpGroup);
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		Eigen::AlignedBox3d mbboxi = TargetShapeSegment[i]->bbox();

		SymmetryGroup tmpGroup;
		tmpGroup.members.push_back(TargetShapeSegment[i]);
		tmpGroup.labels.push_back(i);

		for (int j = i + 1; j < TargetShapeSegmentNum; j++)
		{
			double sum = 0;
			Eigen::AlignedBox3d mbboxj = TargetShapeSegment[j]->bbox();
			sum += abs(mbboxi.sizes()[0] - mbboxj.sizes()[0]);
			sum += abs(mbboxi.sizes()[1] - mbboxj.sizes()[1]);
			sum += abs(mbboxi.sizes()[2] - mbboxj.sizes()[2]);

			double threshold = std::min(mbboxi.sizes()[0] + mbboxi.sizes()[1] + mbboxi.sizes()[2], mbboxj.sizes()[0] + mbboxj.sizes()[1] + mbboxj.sizes()[2]);
			if (sum / threshold < t)
			{
				tmpGroup.members.push_back(TargetShapeSegment[j]);
				tmpGroup.labels.push_back(j);
			}
		}

		bool isAdded = false;
		for (int j = 0; j < TargetSegGroups.size(); j++)
		{
			int matched = 0;
			for (int k = 0; k < tmpGroup.labels.size(); k++)
			{
				if (std::find(TargetSegGroups[j].labels.begin(), TargetSegGroups[j].labels.end(), tmpGroup.labels[k]) != TargetSegGroups[j].labels.end())
					matched++;
			}
			if (matched == tmpGroup.labels.size())
			{
				isAdded = true;
				break;
			}
		}
		if (!isAdded)
			TargetSegGroups.push_back(tmpGroup);
	}

}*/

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
			SourceShapeSegmentAxisDirection[i][j] = direction;
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
			TargetShapeSegmentAxisDirection[i][j] = direction;
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

	GetSegFaceNum();

	GenerateSegMeshes(0);
	GenerateSegMeshes(1);

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