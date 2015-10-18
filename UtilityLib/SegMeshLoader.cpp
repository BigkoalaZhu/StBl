#include "SegMeshLoader.h"
#include <QFile>

SegMeshLoader::SegMeshLoader( SurfaceMesh::SurfaceMeshModel * mesh )
{
	this->entireMesh = mesh;
	entirePoints = entireMesh->vertex_property<Vector3>("v:point");
}

void SegMeshLoader::loadGroupsFromObj()
{
	// Read obj file
	QFile file(entireMesh->path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
	QTextStream inF(&file);

	int fidx = 0;
	while( !inF.atEnd() )
	{
		QString line = inF.readLine();
		if(line.isEmpty()) continue;

		if (line.startsWith("g"))
		{
			QStringList groupLine = line.split(" ");

			QString gid;
			if(!groupLine.isEmpty()) gid = groupLine.at(1);
			else gid = QString::number(groupFaces.size());

			while(true)
			{
				QString line = inF.readLine();
				if(!line.startsWith("f")) break;

				groupFaces[gid].push_back(fidx++);
			}
		}
	}

}


SurfaceMeshModel* SegMeshLoader::extractSegMesh( QString gid )
{
	SurfaceMeshModel* subMesh = new SurfaceMeshModel(gid + ".obj", gid);

	QVector<int> part = groupFaces[gid];

	// vertex set from face
	QSet<int> vertSet;
	foreach(int fidx, part)
	{
		Surface_mesh::Vertex_around_face_circulator vit = entireMesh->vertices(Face(fidx)),vend=vit;
		do{ vertSet.insert(Vertex(vit).idx()); } while(++vit != vend);
	}

	// entire vid => segment vid
	QMap<Vertex,Vertex> vmap;
	foreach(int vidx, vertSet)
	{
		vmap[Vertex(vidx)] = Vertex(vmap.size());
		subMesh->add_vertex( entirePoints[Vertex(vidx)] );
	}

	// copy vertices to subMesh
	foreach(int fidx, part){
		std::vector<Vertex> pnts; 
		Surface_mesh::Vertex_around_face_circulator vit = entireMesh->vertices(Face(fidx)),vend=vit;
		do{ pnts.push_back(vmap[vit]); } while(++vit != vend);
		subMesh->add_face(pnts);
	}

	subMesh->updateBoundingBox();
	subMesh->isVisible = true;

	return subMesh;
}

QVector<SurfaceMeshModel*> SegMeshLoader::getSegMeshes()
{
	/*loadGroupsFromObj();

	QVector<SurfaceMeshModel*> meshes;
	foreach (QString gid, groupFaces.keys())
	meshes.push_back(extractSegMesh(gid));
	return meshes;*/

	return loadSegmentedOBJ(entireMesh->path);
}

QVector<SurfaceMeshModel*> SegMeshLoader::loadSegmentedOBJ(QString filename)
{
	QVector<SurfaceMeshModel*> sub_meshes;

	// Read obj file
	QFile file( filename );
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return sub_meshes;
	QTextStream inF(&file);

	bool isLoadingGroup = false;
	int voffset = 0;

	typedef std::vector<Vertex> VectorVertices;

	QVector< Vector3 > vertices;
	QVector< VectorVertices > faces;
	QString groupName;

	while( !inF.atEnd() )
	{
		QString line = inF.readLine();
		if(line.isEmpty()) continue;

		if(isLoadingGroup && line.startsWith("v "))
		{
			// Create the mesh we have right now
			sub_meshes.push_back(new SurfaceMeshModel(groupName + ".obj", groupName));

			foreach(Vector3 p, vertices) sub_meshes.back()->add_vertex(p);
			foreach(VectorVertices f, faces) sub_meshes.back()->add_face(f);

			voffset += vertices.size();

			// Clear containers 'vertices' and 'faces'
			vertices.clear();
			faces.clear();

			isLoadingGroup = false;
		}

		if (line.startsWith("v "))
		{
			QStringList vLine = line.split(" ", QString::SkipEmptyParts);

			double x = vLine[1].toDouble();
			double y = vLine[2].toDouble();
			double z = vLine[3].toDouble();

			vertices.push_back(Vector3(x,y,z));
		}

		if (line.startsWith("f "))
		{
			QStringList fLine = line.split(" ", QString::SkipEmptyParts);

			int v1, v2, v3;

			int idx = fLine[1].indexOf("/");
			if (idx == -1)
			{
				v1 = (fLine[1].toInt() - 1) - voffset;
				v2 = (fLine[2].toInt() - 1) - voffset;
				v3 = (fLine[3].toInt() - 1) - voffset;
			}
			else
			{
				v1 = (fLine[1].left(idx).toInt() - 1) - voffset;
				idx = fLine[2].indexOf("/");
				v2 = (fLine[2].left(idx).toInt() - 1) - voffset;
				idx = fLine[3].indexOf("/");
				v3 = (fLine[3].left(idx).toInt() - 1) - voffset;
			}	

			if (v1 == v2 || v1 == v3 || v2 == v3)
			{
				continue;
			}

			std::vector<Vertex> verts;
			verts.push_back(Vertex(v1));
			verts.push_back(Vertex(v2));
			verts.push_back(Vertex(v3));			
			faces.push_back(verts);
		}

		if (line.startsWith("g "))
		{
			isLoadingGroup = true;

			groupName = line.split(" ", QString::SkipEmptyParts).at(1);
		}
	}

	// Last part
	if(vertices.size() && faces.size())
	{
		sub_meshes.push_back(new SurfaceMeshModel(groupName + ".obj", groupName));
		foreach(Vector3 p, vertices) sub_meshes.back()->add_vertex(p);
		foreach(VectorVertices f, faces) sub_meshes.back()->add_face(f);
	}

	return sub_meshes;
}
