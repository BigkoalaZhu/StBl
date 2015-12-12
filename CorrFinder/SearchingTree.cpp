#include "SearchingTree.h"


SearchingTree::SearchingTree(SearchingNode * r, double vt, double jt, int m, int n, QVector<double> Va, QVector<double> Vb, QVector<QVector<int>> BA, QVector<QVector<int>> BB)
{
	root = r;
	sTree.clear();
	sTree.push_back(r);

	volumeThreshold = vt;
	jumpOutThreshold = jt;
	totalA = m;
	totalB = n;

	VolumeListA = Va;
	VolumeListB = Vb;
	VolumeTotal = 0;
	for each (double v in VolumeListA)
		VolumeTotal += v;
	for each (double v in VolumeListB)
		VolumeTotal += v;
	BanListA = BA;
	BanListB = BB;

	energyThreshold = 1.0;
}


SearchingTree::~SearchingTree()
{
}

void SearchingTree::evaluateNode(SearchingNode * node)
{

}

void SearchingTree::choosenParts(SearchingNode * node)
{
	for (int i = 0; i < node->corr.size(); i++)
	{
		for (int j = 0; j < BanListA[node->corr[i].first].size(); j++)
			node->choosenA.push_back(BanListA[node->corr[i].first][j]);
		for (int j = 0; j < BanListB[node->corr[i].second].size(); j++)
			node->choosenB.push_back(BanListB[node->corr[i].second][j]);
	}
}

bool SearchingTree::expandBranch(SearchingNode * node)
{
	SearchingNode * newNode = new SearchingNode;
	newNode->currentPairindex = node->currentPairindex;
	newNode->corr = node->corr;
	newNode->corr.erase(newNode->corr.begin() + node->currentPairindex);
	choosenParts(newNode);
	int validB = node->currentPair.second + 1;
	while (!newNode->choosenB.contains(validB))
	{
		validB++;
		if (validB > totalB - 1)
			return false;
	}
	newNode->currentPair = QPair<int, int>(node->currentPair.first, validB);
	newNode->corr.insert(newNode->corr.begin() + node->currentPairindex, newNode->currentPair);
	newNode->parent = node->parent;
	newNode->pre = node;
	node->next = newNode;
	double volumetmp = node->VolumeProportion * VolumeTotal;
	newNode->VolumeProportion = (volumetmp - VolumeListB[node->currentPair.second] + VolumeListB[newNode->currentPair.second]) / VolumeTotal;
	sTree.push_back(newNode);
	return true;
}

bool SearchingTree::expandChild(SearchingNode * node)
{
	if (!node->valid)
		return false;
	SearchingNode * newNode = new SearchingNode;
	newNode->currentPairindex = node->currentPairindex + 1;
	newNode->corr = node->corr;
	choosenParts(newNode);

	int validA = 0;
	while (!newNode->choosenA.contains(validA))
	{
		validA++;
		if (validA > totalA - 1)
			return false;
	}

	newNode->currentPair = QPair<int, int>(node->currentPair.first + 1, validA);
	newNode->corr.push_back(newNode->currentPair);
	newNode->parent = node;
	double volumetmp = node->VolumeProportion * VolumeTotal;
	newNode->VolumeProportion = (volumetmp + VolumeListA[newNode->currentPair.first] + VolumeListB[0]) / VolumeTotal;
	sTree.push_back(newNode);
	return true;
}

QVector<QVector<QPair<int, int>>> SearchingTree::searchValidCorr()
{
	QVector<QVector<QPair<int, int>>> candidates;

	//Expand tree
	while (1)
	{
		SearchingNode * LastOne = sTree[sTree.size() - 1];
		if (LastOne->VolumeProportion > volumeThreshold)
		{
			evaluateNode(LastOne);
			if (LastOne->energy < energyThreshold)
				candidates.push_back(LastOne->corr);
			else
				LastOne->valid = false;
		}
		if (LastOne->currentPair.second < totalB - 1 && expandBranch(LastOne))
		{
			continue;
		}
		if (LastOne->currentPair.first < totalA - 1)
		{
			if (LastOne->parent != NULL)
			{
				if (LastOne->parent->next != NULL)
				{
					expandChild(LastOne->parent->next);
					continue;
				}
			}
			SearchingNode * parent = LastOne->pre;
			if (parent == NULL)
				break;
			while (parent->pre != NULL)
				parent = parent->pre;
			expandChild(parent);
			continue;
		}
	}

	return candidates;
}
