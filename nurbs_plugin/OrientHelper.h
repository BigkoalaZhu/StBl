#pragma once

#include <QMap>
#include <QDebug>

struct OrientHelper{
	int vNum;
	QVector<QVector<int> > faces;
	QVector<bool> isOriented;

	QVector<QVector<int> > adjacency; // the adjacent faces for each face
	QVector< QVector< QPair<int, int> > >  edges; // the edge between each pair of adjacent faces

	QVector<QVector<int> > reorient(std::vector< std::vector<int> > bad_faces, int vNum)
	{
		for (auto f : bad_faces) 
			faces.push_back(QVector<int>::fromStdVector(f));

		this->vNum = vNum;

		getAdjacency();

		isOriented.resize(faces.size());
		isOriented[0] = true;
		reorientNeighbors(0);

		return this->faces;
	}

	void getAdjacency()
	{
		// 1. find all the edges and the faces sharing the same edge
		QMap<QPair<int, int>, QVector<int> > edge2faces;
		for (int i = 0; i < faces.size(); i++)
		{
			for (int j = 0; j < faces[i].size(); j++)
			{
				int k = (j + 1) % faces[i].size();

				int vIdx1 = faces[i][j];
				int vIdx2 = faces[i][k];

				if (vIdx1 > vIdx2)
				{
					int temp = vIdx1;
					vIdx1 = vIdx2;
					vIdx2 = temp;
				}

				QPair<int, int> edge(vIdx1, vIdx2);

				QMap<QPair<int, int>, QVector<int> >::iterator imap = edge2faces.find(edge);
				if (imap == edge2faces.end())
				{
					QVector<int> f;
					f.push_back(i);

					edge2faces.insert(edge, f);
				}
				else
				{
					imap.value().push_back(i);

					if (imap.value().size() > 2)
					{
						qDebug() << "ERROR: more than two faces share an edge!!!";
					}
				}
			}
		}


		// 2. get adjacent faces and the sharing edge
		adjacency.resize(faces.size());
		edges.resize(faces.size());
		for (QMap<QPair<int, int>, QVector<int> >::iterator imap = edge2faces.begin(); imap != edge2faces.end(); imap++)
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
				qDebug() << "ERROR: edge shared by more than three faces!!!";
			}
		}
	}

	void reorientNeighbors(int f)
	{
		for (int i = 0; i < adjacency[f].size(); i++)
		{
			int neighF = adjacency[f][i];
			if (!isOriented[neighF])
			{
				reorientFace(i, f);
				reorientNeighbors(neighF);
			}
		}
	}

	void reorientFace(int neighIdx, int f)
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
			for (int i = faces[neighF].size() - 1; i >= 0; i--)
			{
				reverFace.push_back(faces[neighF][i]);
			}
			faces[neighF] = reverFace;
		}

		isOriented[neighF] = true;
	}
};
