#pragma once

#include <Eigen/Core>
#include <qvector.h>
#include <qstring.h>
#include "SurfaceMeshModel.h"
using namespace SurfaceMesh;

#include "NURBSCurve.h"
#include "NURBSRectangle.h"
#include "NurbsDraw.h"

#include "SurfaceMeshPlugins.h"
#include "SurfaceMeshHelper.h"
#include "RichParameterSet.h"

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
	QVector<SurfaceMesh::SurfaceMeshModel *> meshes;
	QVector<int> allseg;
	int flat;
	NURBS::NURBSRectangled SheetAxis;
	SegmentGroupFromGraph() : flat(0){}
};

struct InitialStructureGraph {
	QVector<SurfaceMesh::SurfaceMeshModel *> meshes;
	QVector<QVector<int>> indexes;
	QVector<QVector<Eigen::Vector3d>> curves;
	QVector<std::vector<std::vector<Vector3d>>> sheets;
	QVector<int> flatIndex;
	QVector<QPair<int, int>> edges;
	QVector<Eigen::Vector4d> edgeCoordA;
	QVector<Eigen::Vector4d> edgeCoordB;
	QVector<QVector<int>> groups;
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

	Eigen::MatrixXd AdjacencySourceGraphGroups;
	Eigen::MatrixXd AdjacencyTargetGraphGroups;
	QVector<SegmentGroupFromGraph> SourceGraphGroups;
	QVector<SegmentGroupFromGraph> TargetGraphGroups;
	QVector<InitialStructureGraph> SourceStructureGraphs;
	QVector<InitialStructureGraph> TargetStructureGraphs;

	QVector<SegmentGroupFromGraph> SourceUnoverlapGraphGroups;
	QVector<SegmentGroupFromGraph> TargetUnoverlapGraphGroups;

	QVector<SegmentGroup> SourceSegGroups;
	QVector<SegmentGroup> TargetSegGroups;

	InitialStructureGraph SourceStructureGraph;
	InitialStructureGraph TargetStructureGraph;

private:
	///////////////////////////////  Functions
	bool LoadParialPartFile();
	void ApplySegmentColor(int sindex = -1);
	void ApplyPartColor(int sindex = -1);
	void ApplySeg(int SorT);

	void FindSegAdjacencyMatrix(int ignoreJoint = 0);
	void FlatSegMerge(double threshold, int SorT, QVector<int> &flat);
	void MergeTwoSegs(int A, int B, int SorT);
	void GenerateSegMeshes(int SorT);
	void GetSegFaceNum();
	void GenerateInitialGroups(double t);
	void MergeStraightConnectedCylinders(int SorT); // Seems useless
	bool IsFlatMerge(int indexA, int indexB, int SorT, double threshold);

	void GenerateGroupsFromGraph();
	bool IsSmoothConnected(QVector<Eigen::Vector3d> PosA, QVector<Eigen::Vector3d> PosB, QVector<Eigen::Vector3d> DirA, QVector<Eigen::Vector3d> DirB, int &type, double threshold = 0.8);
	bool IsSmoothConnected(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> align, QVector<int> &type, double threshold = 0.0);
	bool IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT, QVector<int> &align);
	bool IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT);
	SegmentGroupFromGraph MergeGroups(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> type, QVector<int> align);
	void MergeGraphSegToParts(int SorT);
	bool IsExistedGroups(QVector<SegmentGroupFromGraph> groups, SegmentGroupFromGraph test);
	bool IsAdjacented(QVector<int> indexA, QVector<int> indexB, int SorT, int &err);
	bool IsFlat(SegmentGroupFromGraph group, int SorT);

	void filterJoints(int SorT);
	void generateSheetPara();

	SurfaceMeshModel * mergedSeg(QVector<int> indexes, int SorT);
	bool IsFlatMerge(SegmentGroup groupA, SegmentGroup groupB);
	bool IsSmoothConnected(SegmentGroup groupA, SegmentGroup groupB, int &type, double threshold = 0.8);
	bool IsAdjacented(SegmentGroup groupA, SegmentGroup groupB);
	bool IsExistedGroups(QVector<SegmentGroup> groups, SegmentGroup test);
	void MergeSegToParts(int SorT);
	SegmentGroup MergeGroups(SegmentGroup groupA, SegmentGroup groupB, int type);

	void buildStructureGraph();
	void samplePartialGraphs(int step = 3, int totalNum = 100);
	bool insertNodeInGraph(InitialStructureGraph &graph, SegmentGroupFromGraph node, int SorT);

	NURBS::NURBSRectangled surfaceFit(SurfaceMeshModel * part);
	std::vector<Vertex> collectRings(SurfaceMeshModel * part, Vertex v, size_t min_nb);
	
	///////////////////////////////  Variants
	double threshouldGC;
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

	QVector<double> SourceShapeSegmentGC;
	QVector<double> TargetShapeSegmentGC;

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

