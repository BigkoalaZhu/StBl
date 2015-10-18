
#include "OrientHelper.h"
#include <QMap>
#include <QStack>
#include <QDebug>
#include "UtilityGlobal.h"

OrientHelper::OrientHelper()
{

}

QVector<QVector<int>> OrientHelper::reorient( QVector<QVector<int>> faces, int vNum)
{
	this->faces = faces;
	this->vNum = vNum;

	getAdjacency();
	
	isOriented.resize(faces.size());

	int idx = findUnorientedFace();

	while ( idx != -1 ) // a small bug needs to be fixed: the orientation of second component may be consistent with the first one...
	{
		isOriented[idx] = true;

		QStack<QPair<int, int>> tStack;
		for (int i=0; i<adjacency[0].size(); i++)
		{
			tStack.push(QPair<int, int>(i, 0));
		}

		while (!tStack.isEmpty())
		{
			QPair<int, int> t = tStack.pop();
			reorientFace(t.first, t.second);	

			int f = adjacency[t.second][t.first];
			for (int i=0; i<adjacency[f].size(); i++)
			{
				int neighF = adjacency[f][i];
				if (!isOriented[neighF])
				{
					tStack.push(QPair<int, int>(i, f));			
				}
			}
		}

		idx = findUnorientedFace();
	}

	
	//// other option: use recursive algorithm
	//reorientNeighbors(0);

	return this->faces;
}

void OrientHelper::getAdjacency( )
{
	// 1. find all the edges and the faces sharing the same edge
	QMap<QPair<int, int>, QVector<int>> edge2faces;
	for (int i=0; i<faces.size(); i++)
	{
		for (int j=0; j<faces[i].size(); j++)
		{
			int k = (j+1) % faces[i].size();

			int vIdx1 = faces[i][j];
			int vIdx2 = faces[i][k];

			if(vIdx1 > vIdx2)
			{
				int temp = vIdx1;
				vIdx1 = vIdx2;
				vIdx2 = temp;
			}

			QPair<int, int> edge(vIdx1, vIdx2);

			QMap<QPair<int, int>, QVector<int>>::iterator imap = edge2faces.find(edge);
			if (imap == edge2faces.end())
			{
				QVector<int> f;
				f.push_back(i);

				edge2faces.insert(edge, f);
			}
			else
			{
				imap.value().push_back(i);

				if(imap.value().size() > 2)
				{
					qDebug() << "ERROR: more than two faces share an edge!!!";
				}
			}
		}
	}

		
	// 2. get adjacent faces and the sharing edge
	adjacency.resize(faces.size());
	edges.resize(faces.size());
	for (QMap<QPair<int, int>, QVector<int>>::iterator imap=edge2faces.begin(); imap!=edge2faces.end(); imap++)
	{
		QVector<int> adjFaces = imap.value();

		if (adjFaces.size() == 2)
		{
			adjacency[adjFaces[0]].push_back(adjFaces[1]);
			adjacency[adjFaces[1]].push_back(adjFaces[0]);

			edges[adjFaces[0]].push_back(imap.key());
			edges[adjFaces[1]].push_back(imap.key());
		}
		else if (adjFaces.size() == 1)
		{
			//qDebug() << "edge on the boundary.";
		}
		else
		{
			debugBox( "ERROR: edge shared by more than two faces!!!" );
		}
	}
}

void OrientHelper::reorientNeighbors( int f )
{
	for (int i=0; i<adjacency[f].size(); i++)
	{
		int neighF = adjacency[f][i];
		if (!isOriented[neighF])
		{
			reorientFace(i, f);		
			reorientNeighbors(neighF);
		}
	}
}

void OrientHelper::reorientFace( int neighIdx, int f )
{
	int neighF = adjacency[f][neighIdx];		
	QPair<int, int> edge = edges[f][neighIdx];

	int order1 = faces[f].indexOf(edge.first) - faces[f].indexOf(edge.second);
	order1 = order1 > 1 ? -1 : order1;
	order1 = order1 < -1 ? 1 : order1;
	int order2 = faces[neighF].indexOf(edge.first) - faces[neighF].indexOf(edge.second);
	order2 = order2 > 1 ? -1 : order2;
	order2 = order2 < -1 ? 1 : order2;

	if (order1 * order2 > 0)
	{
		QVector<int> reverFace;
		for (int i = faces[neighF].size()-1; i>=0; i--)
		{
			reverFace.push_back(faces[neighF][i]);
		}
		faces[neighF] = reverFace;

		//for (int i=0; i<faces[neighF].size()/2; i++)
		//{
		//	int tmp = faces[neighF][i];
		//	faces[neighF][i] = faces[neighF][faces[neighF].size() - 1 - i];
		//	faces[neighF][faces[neighF].size() - 1 - i] = tmp;
		//}
	}

	isOriented[neighF] = true;
}

int OrientHelper::findUnorientedFace()
{
	int idx = -1;
	for (int i=0; i<isOriented.size(); i++)
	{
		if (!isOriented[i])
		{
			idx = i;
			break;
		}
	}

	return idx;
}



