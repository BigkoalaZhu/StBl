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

struct SegmentGroupFromGraph {
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<int>> labels;
	QVector<QVector<int>> joints;
};

class CorrFinder
{
public:
	CorrFinder();
	~CorrFinder();

	bool LoadPairFile(QString filepath, bool hasPart = false, bool hasInbetween = false);
	void DrawPartShape();
	void GeneratePartSet();
	void DrawSpecificPart(int index, int SorT);

	SurfaceMeshModel * getSourceShape(){ return SourceShape; }
	SurfaceMeshModel * getTargetShape(){ return TargetShape; }

	QVector<SegmentGroupFromGraph> SourceGraphGroups;
	QVector<SegmentGroupFromGraph> TargetGraphGroups;

	QVector<SegmentGroup> SourceSegGroups;
	QVector<SegmentGroup> TargetSegGroups;

private:
	///////////////////////////////  Functions
	bool LoadParialPartFile();
	void ApplySegmentColor(int sindex = -1);
	void ApplyPartColor(int sindex = -1);
	void ApplySeg(int SorT);

	void FindSegAdjacencyMatrix();
	void FlatSegMerge(double threshold, int SorT, QVector<int> &flat);
	void MergeTwoSegs(int A, int B, int SorT);
	void GenerateSegMeshes(int SorT);
	void GetSegFaceNum();
	void GenerateInitialGroups(double t);
	void MergeStraightConnectedCylinders(int SorT); // Seems useless
	bool IsFlatMerge(int indexA, int indexB, int SorT);

	void GenerateGroupsFromGraph();
	bool IsSmoothConnected(QVector<Eigen::Vector3d> PosA, QVector<Eigen::Vector3d> PosB, QVector<Eigen::Vector3d> DirA, QVector<Eigen::Vector3d> DirB, int &type, double threshold = 0.8);
	bool IsSmoothConnected(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> align, QVector<int> &type, double threshold = 0.0);
	bool IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT, QVector<int> &align);
	SegmentGroupFromGraph MergeGroups(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> type, QVector<int> align);
	void MergeGraphSegToParts(int SorT);
	bool IsExistedGroups(QVector<SegmentGroupFromGraph> groups, SegmentGroupFromGraph test);
	bool IsAdjacented(QVector<int> indexA, QVector<int> indexB, int SorT, int &err);
	bool IsFlat(SegmentGroupFromGraph group, int SorT);

	SurfaceMeshModel * mergedSeg(QVector<int> indexes, int SorT);
	bool IsFlatMerge(SegmentGroup groupA, SegmentGroup groupB);
	bool IsSmoothConnected(SegmentGroup groupA, SegmentGroup groupB, int &type, double threshold = 0.8);
	bool IsAdjacented(SegmentGroup groupA, SegmentGroup groupB);
	bool IsExistedGroups(QVector<SegmentGroup> groups, SegmentGroup test);
	void MergeSegToParts(int SorT);
	SegmentGroup MergeGroups(SegmentGroup groupA, SegmentGroup groupB, int type);
	
	///////////////////////////////  Variants
	QVector< QColor > ColorDifferent;
	QVector< QColor > ColorMap;

	QString pairfile_path;
	QString sourceShape_path, sourceIndex_path;
	QString targetShape_path, targetIndex_path;

	SurfaceMeshModel * SourceShape;
	SurfaceMeshModel * TargetShape;

	int sourceVaildSegNum, targetVaildSegNum;

	Eigen::MatrixXd SourceSegAdjacencyMatrix;
	Eigen::MatrixXd TargetSegAdjacencyMatrix;

	Eigen::MatrixXd SourceRealSegAdjacencyMatrix;
	Eigen::MatrixXd TargetRealSegAdjacencyMatrix;

	
	QVector<int> SourceShapeSegmentJointIndex; //yes:1, no:-1
	QVector<int> TargetShapeSegmentJointIndex;

	QVector<int> SourceShapeSegmentFlatIndex;
	QVector<int> TargetShapeSegmentFlatIndex;

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
};

