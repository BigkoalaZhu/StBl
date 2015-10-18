#pragma once

#include "SurfaceMeshPlugins.h"
#include <QtCore>

class writeOBJ
{
public:
	static void wirte(const SurfaceMeshModel* mesh, const QString filename)
	{
		QFile file(filename);
		file.open(QIODevice::ReadWrite | QIODevice::Text);
		QTextStream input(&file);
		input << "#Exported by StructureBlending." << endl;
		input << "#Written by Chenyang Zhu." << endl;

		int Vnum = 0;
		SurfaceMeshModel::Vertex_property<SurfaceMeshModel::Point> points = mesh->get_vertex_property<Point>("v:point");
		for (SurfaceMeshModel::Vertex_iterator vit=mesh->vertices_begin(); vit!=mesh->vertices_end(); ++vit)
		{
			const Point& p = points[vit];
			input<< "v " << p[0] << " " << p[1] << " " << p[2] << endl;
			Vnum++;
		}
		input << "# " << Vnum << " vertices" << endl << endl;

		Vnum = 0;
		SurfaceMeshModel::Vertex_property<SurfaceMeshModel::Point> npoints = mesh->get_vertex_property<Point>("v:normal");
		for (SurfaceMeshModel::Vertex_iterator vit=mesh->vertices_begin(); vit!=mesh->vertices_end(); ++vit)
		{
			const Point& p = npoints[vit];
			input<< "vn " << p[0] << " " << p[1] << " " << p[2] << endl;
			Vnum++;
		}
		input << "# " << Vnum << " vertice normals" << endl << endl;

		Vnum = 0;
		for (SurfaceMeshModel::Face_iterator fit=mesh->faces_begin(); fit!=mesh->faces_end(); ++fit)
		{
			SurfaceMeshModel::Vertex_around_face_circulator fvit=mesh->vertices(fit), fvend=fvit;
			input << "f ";
			do
			{
				input << ((SurfaceMeshModel::Vertex)fvit).idx()+1 << "//" << ((SurfaceMeshModel::Vertex)fvit).idx()+1 << " ";
			}
			while (++fvit != fvend);
			input << endl;
			Vnum++;
		}
		input << "# " << Vnum << " faces" << endl;
	}
};