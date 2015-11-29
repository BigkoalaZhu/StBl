#include "CorrFinder.h"
#include "Colormaps.h"
#include "QuickMeshDraw.h"
#include <qfile.h>

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

	QTextStream in(&pairfile);
	QString str = in.readLine();
	QStringList strtmp = str.split(" ");
	sourceShape_path = strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetShape_path = strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	sourceIndex_path = strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetIndex_path = strtmp[1];

	if (hasPart)
	{
		str = in.readLine();
		strtmp = str.split(" ");
		int PartNum = strtmp[1].toInt();
		SourceShapePartIndex.resize(PartNum);
		for (int i = 0; i < PartNum; i++)
			SourceShapePartIndex[i] = strtmp[i + 2].toInt();

		str = in.readLine();
		strtmp = str.split(" ");
		PartNum = strtmp[1].toInt();
		TargetShapePartIndex.resize(PartNum);
		for (int i = 0; i < PartNum; i++)
			TargetShapePartIndex[i] = strtmp[i + 2].toInt();
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

	TargetShape = new SurfaceMeshModel(targetShape_path);
	TargetShape->read(targetShape_path.toStdString());
	TargetShape->updateBoundingBox();
	TargetShape->update_face_normals();
	TargetShape->update_vertex_normals();
	TargetShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));

	SourceShapeSegmentIndex.resize(SourceShape->faces_size());
	SourceShapePartIndex.resize(SourceShape->faces_size());

	TargetShapeSegmentIndex.resize(TargetShape->faces_size());
	TargetShapePartIndex.resize(TargetShape->faces_size());

	if (!LoadParialPartFile())
		return false;

	ApplySegmentColor();
	return true;
}

void CorrFinder::ApplySegmentColor()
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();

	double eachInternal = 1.0f / sourceVaildSegNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		if (SourceShapeSegmentJointIndex[SourceShapeSegmentIndex[index]] == -1)
		{
			index++;
			continue;
		}
		fscolors[fit] = getColorFromMap(eachInternal*SourceShapeSegmentIndex[index], ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();
	
	eachInternal = 1.0f / targetVaildSegNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		if (TargetShapeSegmentJointIndex[TargetShapeSegmentIndex[index]] == -1)
		{
			index++;
			continue;
		}
		ftcolors[fit] = getColorFromMap(eachInternal*TargetShapeSegmentIndex[index], ColorMap);
		index++;
	}
}

void CorrFinder::FindSegAdjacencyMatrix()
{
}

void CorrFinder::ApplyPartColor()
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();
	double eachInternal = 1.0f / SourceShapePartNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		if (SourceShapeSegmentJointIndex[index] == -1)
			continue;
		fscolors[fit] = getColorFromMap(eachInternal*SourceShapePartIndex[index], ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();
	eachInternal = 1.0f / TargetShapePartNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		if (TargetShapeSegmentJointIndex[index] == -1)
			continue;
		ftcolors[fit] = getColorFromMap(eachInternal*TargetShapePartIndex[index], ColorMap);
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

		for (int j = 0; j < faceNum; j++)
		{
			SourceShapeSegmentIndex[strtmp[j].toInt()] = i;
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

		for (int j = 0; j < faceNum; j++)
		{
			TargetShapeSegmentIndex[strtmp[j].toInt()] = i;
		}
	}

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

	return true;
}