#pragma once

#include <QtCore>
#include "Eigen/Dense"

struct SegGraphNode
{
	QVector<int> labels;
	double lowest;
	int level;
	int inNum;
	int outNum;
	Eigen::Vector3d feature;
	SegGraphNode() : lowest(0), level(-1), inNum(0), outNum(0), feature(Eigen::Vector3d(0,0,0)){}
};

struct SegGraphEdge
{
	SegGraphNode from;
	SegGraphNode to;
};

class SegGraph
{
public:
	SegGraph();
	~SegGraph();

	void AddNode(SegGraphNode node);
	void AddAdjacencyMatrix(Eigen::MatrixXd m){ AdjacencyMatrix = m; }
	void BuildInitialGraph();
	void GenerateGroups();
	void OutputInitialGraph();

	QVector<QVector<int>> groups;
private:
	bool featureMatch(Eigen::Vector3d f1, Eigen::Vector3d f2);

	Eigen::MatrixXd AdjacencyMatrix;
	QVector<SegGraphNode> Nodes;
	QVector<SegGraphEdge> Edges;
};

