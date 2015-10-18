#pragma once

#include "UtilityGlobal.h"

class SegMeshLoader
{
public:
    SegMeshLoader(SurfaceMeshModel * mesh);

	// Entire mesh
	SurfaceMeshModel* entireMesh;
	Vector3VertexProperty entirePoints;

	// Groups
	QMap< QString, QVector<int> > groupFaces;

	void loadGroupsFromObj();
	SurfaceMeshModel* extractSegMesh( QString gid );
	QVector<SurfaceMeshModel*> getSegMeshes();

	//Directly read segments from OBJ
	QVector<SurfaceMeshModel*> loadSegmentedOBJ(QString filename);
};

