#pragma once

#include <QVector>
#include <QPair>

class OrientHelper
{
public:
	OrientHelper();

public:
	QVector<QVector<int>> reorient(QVector<QVector<int>> faces, int vNum);

private:
	void getAdjacency();
	void reorientNeighbors(int f);
	void reorientFace(int neighIdx, int f); // reorient the neighbor based on the orientation of f
	int findUnorientedFace();

private:
	int vNum;
	QVector<QVector<int>> faces;
	QVector<bool> isOriented;

	QVector<QVector<int>> adjacency; // the adjacent faces for each face
	QVector< QVector< QPair<int, int> > >  edges; // the edge between each pair of adjacent faces
};