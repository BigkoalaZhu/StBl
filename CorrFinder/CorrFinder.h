#pragma once

#include <Eigen/Core>
#include <qvector.h>
#include <qstring.h>
#include "SurfaceMeshModel.h"
using namespace SurfaceMesh;

class CorrFinder
{
public:
	CorrFinder();
	~CorrFinder();

	bool LoadPairFile(QString filepath, bool hasPart = false, bool hasInbetween = false);
	void DrawPartShape();

	SurfaceMeshModel * getSourceShape(){ return SourceShape; }
	SurfaceMeshModel * getTargetShape(){ return TargetShape; }

private:
	///////////////////////////////  Functions
	bool LoadParialPartFile();
	void ApplySegmentColor();
	void ApplyPartColor();

	void FindSegAdjacencyMatrix();


	///////////////////////////////  Variants
	QVector< QColor > ColorMap;

	QString pairfile_path;
	QString sourceShape_path, sourceIndex_path;
	QString targetShape_path, targetIndex_path;

	SurfaceMeshModel * SourceShape;
	SurfaceMeshModel * TargetShape;

	int sourceVaildSegNum, targetVaildSegNum;

	Eigen::MatrixXd SourceSegAdjacencyMatrix;
	Eigen::MatrixXd TargetSegAdjacencyMatrix;

	int SourceShapeSegmentNum, TargetShapeSegmentNum;
	QVector<int> SourceShapeSegmentJointIndex; //yes:1, no:-1
	QVector<int> TargetShapeSegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SourceShapeSegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SourceShapeSegmentAxisDirection;
	QVector<QVector<Eigen::Vector3d>> TargetShapeSegmentAxis;
	QVector<QVector<Eigen::Vector3d>> TargetShapeSegmentAxisDirection;
	QVector<int> SourceShapeSegmentIndex;
	QVector<int> TargetShapeSegmentIndex;
	int SourceShapePartNum, TargetShapePartNum;
	QVector<int> SourceShapePartIndex;
	QVector<int> TargetShapePartIndex;
	QVector<QString> InbetweenShapes;
};

