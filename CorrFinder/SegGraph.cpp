#include "SegGraph.h"
#include <iostream>
#include "UtilityGlobal.h"


SegGraph::SegGraph()
{
}


SegGraph::~SegGraph()
{
}

void SegGraph::AddNode(SegGraphNode node)
{ 
	if (Nodes.size() == 0)
	{
		Nodes.push_back(node);
		return;
	}
		
	for (int i = 0; i < Nodes.size(); i++)
	{
		if (node.lowest < Nodes[i].lowest)
		{
			Nodes.insert(Nodes.begin() + i, node);
			return;
		}
	}
	Nodes.push_back(node);
}

void SegGraph::BuildInitialGraph()
{
	//Set first floor
	int labeled = 0;
	Nodes[0].level = 0;
	labeled++;
	for (int i = 1; i < Nodes.size(); i++)
	{
/*		bool flag = true;
		for (int j = 0; j < i; j++)
		{
			if (Nodes[j].level == 0 && AdjacencyMatrix(i, j) == 1)
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			Nodes[i].level = 0;
			labeled++;
		}*/
		if (abs(Nodes[i].lowest - Nodes[0].lowest) < 0.01)
		{
			Nodes[i].level = 0;
			labeled++;
		}//May cause bugs
			
	}

	int level = 0;
	QVector<QPair<int, int>> visited;

	while (labeled < Nodes.size())
	{
		for (int i = 0; i < Nodes.size(); i++)
		{
			if (Nodes[i].level != level)
				continue;
			for (int j = 0; j < Nodes.size(); j++)
			{
				if (i != j &&AdjacencyMatrix(Nodes[i].labels[0], Nodes[j].labels[0]) == 1 && level + 1 >= Nodes[j].level)
				{
					if (visited.contains(QPair<int, int>(i, j)))
						continue;
					if (Nodes[j].level == -1)
						labeled++;
					SegGraphEdge tmp;
					tmp.from = Nodes[i];
					tmp.to = Nodes[j];
					Nodes[i].outNum++;
					Nodes[j].inNum++;
					Edges.push_back(tmp);
					if (level == Nodes[j].level)
					{
						SegGraphEdge tmp_v;
						tmp_v.from = Nodes[j];
						tmp_v.to = Nodes[i];
						Nodes[j].outNum++;
						Nodes[i].inNum++;
						Edges.push_back(tmp_v);
					}
					
					visited.push_back(QPair<int, int>(i, j));
					visited.push_back(QPair<int, int>(j, i));
					if (level > Nodes[j].level)
						Nodes[j].level = level + 1;
				}
			}
		}
		level++;
	}
}

void SegGraph::GenerateGroups()
{
	QVector<int> visited;
	for (int i = 0; i < Nodes.size(); i++)
	{
		if (visited.contains(i))
			continue;
		QVector<int> tmpgroup;
		tmpgroup.push_back(Nodes[i].labels[0]);
		visited.push_back(i);
		for (int j = i + 1; j < Nodes.size(); j++)
		{
			if (visited.contains(j))
				continue;
			if (Nodes[j].level == Nodes[i].level && Nodes[j].inNum == Nodes[i].inNum && Nodes[j].outNum == Nodes[i].outNum)
			{
				if (featureMatch(Nodes[i].feature, Nodes[j].feature))
				{
					tmpgroup.push_back(Nodes[j].labels[0]);
					visited.push_back(j);
				}
			}
		}
		groups.push_back(tmpgroup);
	}
}

bool SegGraph::featureMatch(Eigen::Vector3d f1, Eigen::Vector3d f2)
{
	QVector<QVector<int>> indexes;
	QVector<int> indextmp;
	indextmp.resize(3);
	Permutation(3, indextmp, indexes, 0);
	double minN = 99999;
	for (int i = 0; i < indexes.size(); i++)
	{
		Eigen::Vector3d tmp(f1[indexes[i][0]], f1[indexes[i][1]], f1[indexes[i][2]]);
		if ((tmp - f2).norm() < minN)
			minN = (tmp - f2).norm();
	}
	if (minN / std::min(f1.norm(), f2.norm()) < 1)
		return true;
	else
		return false;
}

void SegGraph::OutputInitialGraph()
{
	QFile file("graph.txt");
	file.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream input(&file);

	for (int i = 0; i < groups.size(); i++)
	{
		for (int j = 0; j < groups[i].size(); j++)
		{
			input << groups[i][j] << " ";
		}
		input << "\n";
	}

	input << "\n";

	for (int i = 0; i < Nodes.size(); i++)
	{
		input<< Nodes[i].labels[0] << " " << Nodes[i].level << " " << Nodes[i].inNum << " " << Nodes[i].outNum << "\n";
	}

	input << "\n";

	for (int i = 0; i < Edges.size(); i++)
	{
		input << Edges[i].from.labels[0] << " " << Edges[i].to.labels[0] << "\n";
	}
	file.close();
}
