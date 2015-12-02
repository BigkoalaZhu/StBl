#pragma once

#include <Eigen/Core>
#include <qvector.h>
#include <qstring.h>
#include "SurfaceMeshModel.h"
using namespace SurfaceMesh;

#include "SymmetryAnalysis.h"

struct SegmentGroup {
	SurfaceMesh::SurfaceMeshModel * members;
	QVector<Eigen::Vector3d> SegmentAxisDirection;
	QVector<Eigen::Vector3d> SegmentAxis;
	QVector<int> labels;
	int SorT;
};

class CorrFinder
{
public:
	CorrFinder();
	~CorrFinder();

	bool LoadPairFile(QString filepath, bool hasPart = false, bool hasInbetween = false);
	void DrawPartShape();
	void GeneratePartSet();

	SurfaceMeshModel * getSourceShape(){ return SourceShape; }
	SurfaceMeshModel * getTargetShape(){ return TargetShape; }

private:
	///////////////////////////////  Functions
	bool LoadParialPartFile();
	void ApplySegmentColor(int sindex = -1);
	void ApplyPartColor(int sindex = -1);
	void ApplySeg(int SorT);

	void FindSegAdjacencyMatrix();
	void FlatSegMerge(double threshold, int SorT);
	void MergeTwoSegs(int A, int B, int SorT);
	void GenerateSegMeshes(int SorT);
	void GetSegFaceNum();
	void GenerateInitialGroups(double t);
	void MergeStraightConnectedCylinders(int SorT); // Seems useless
	bool IsFlatMerge(int indexA, int indexB, int SorT);

	SurfaceMeshModel * mergedSeg(QVector<int> indexes, int SorT);
	bool IsFlatMerge(SurfaceMeshModel * segA, SurfaceMeshModel * segB);
	bool IsSmoothConnected(SegmentGroup groupA, SegmentGroup groupB, double threshold, int &type);
	void MergeSegToParts(int SorT);
	SegmentGroup MergeGroups(SegmentGroup groupA, SegmentGroup groupB, int type);

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

	
	QVector<int> SourceShapeSegmentJointIndex; //yes:1, no:-1
	QVector<int> TargetShapeSegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SourceShapeSegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SourceShapeSegmentAxisDirection;
	QVector<QVector<Eigen::Vector3d>> TargetShapeSegmentAxis;
	QVector<QVector<Eigen::Vector3d>> TargetShapeSegmentAxisDirection;

	int SourceShapeSegmentNum, TargetShapeSegmentNum;
	QVector<int> SourceRealSegIndex;
	QVector<int> TargetRealSegIndex;
	QVector<QVector<int>> SourceShapeSegmentIndex;
	QVector<QVector<int>> TargetShapeSegmentIndex;
	QVector<SurfaceMeshModel *> SourceShapeSegment;
	QVector<SurfaceMeshModel *> TargetShapeSegment;
	QVector<int> SourceSegFaceNum;
	QVector<int> TargetSegFaceNum;

	int SourceShapePartNum, TargetShapePartNum;
	QVector<QVector<int>> SourceShapePartIndex;
	QVector<QVector<int>> TargetShapePartIndex;
	QVector<QString> InbetweenShapes;

	QVector<SegmentGroup> SourceSegGroups;
	QVector<SegmentGroup> TargetSegGroups;
};

