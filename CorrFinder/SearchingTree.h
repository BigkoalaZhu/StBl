#pragma once

#include <QtCore>

struct SearchingNode
{
	QVector<int> choosenA;
	QVector<int> choosenB;
	QVector<QPair<int,int>> corr;
	QPair<int, int> currentPair;
	double VolumeProportion;
	int currentPairindex;
	bool valid;
	double energy;
	SearchingNode *parent;
	SearchingNode *pre;
	SearchingNode *next;
	SearchingNode() : currentPairindex(0), energy(0), valid(true), VolumeProportion(0), parent(NULL), pre(NULL), next(NULL){}
};

class SearchingTree
{
public:
	SearchingTree(SearchingNode * r, double vt, double jt, int m, int n, QVector<double> Va, QVector<double> Vb, QVector<QVector<int>> BA, QVector<QVector<int>> BB);
	~SearchingTree();

	void evaluateNode(SearchingNode * node);
	QVector<QVector<QPair<int, int>>> searchValidCorr();
private:
	bool expandBranch(SearchingNode * node);
	bool expandChild(SearchingNode * node);
	void choosenParts(SearchingNode * node);

	int totalA, totalB;
	double volumeThreshold;
	double jumpOutThreshold;
	double energyThreshold;
	QVector<double> VolumeListA;
	QVector<double> VolumeListB;
	double VolumeTotal;

	QVector<QVector<int>> BanListA;
	QVector<QVector<int>> BanListB;
	SearchingNode * root;
	QVector<SearchingNode *> sTree;
};

