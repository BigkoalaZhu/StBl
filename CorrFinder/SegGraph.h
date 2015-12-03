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
	SegGraphNode() : lowest(0), level(-1), inNum(0), outNum(0){}
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
	void OutputInitialGraph();

private:
	Eigen::MatrixXd AdjacencyMatrix;
	QVector<SegGraphNode> Nodes;
	QVector<SegGraphEdge> Edges;
};

