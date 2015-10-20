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

	static void wirte(const std::vector< SurfaceMesh::SurfaceMeshModel* > meshes, const QString filename)
	{
		QFile file(filename);
		file.open(QIODevice::ReadWrite | QIODevice::Text);
		QTextStream input(&file);
		input << "#Exported by StructureBlending." << endl;
		input << "#Written by Chenyang Zhu." << endl;

		int totalNum = 0;
		for (int part = 0; part < meshes.size(); part++)
		{
			int Vnum = 0;
			SurfaceMeshModel::Vertex_property<SurfaceMeshModel::Point> points = meshes[part]->get_vertex_property<Point>("v:point");
			for (SurfaceMeshModel::Vertex_iterator vit = meshes[part]->vertices_begin(); vit != meshes[part]->vertices_end(); ++vit)
			{
				const Point& p = points[vit];
				input << "v " << p[0] << " " << p[1] << " " << p[2] << endl;
				Vnum++;
			}
			input << "# " << Vnum << " vertices" << endl << endl;

			Vnum = 0;
			SurfaceMeshModel::Vertex_property<SurfaceMeshModel::Point> npoints = meshes[part]->get_vertex_property<Point>("v:normal");
			for (SurfaceMeshModel::Vertex_iterator vit = meshes[part]->vertices_begin(); vit != meshes[part]->vertices_end(); ++vit)
			{
				const Point& p = npoints[vit];
				input << "vn " << p[0] << " " << p[1] << " " << p[2] << endl;
				Vnum++;
			}
			input << "# " << Vnum << " vertice normals" << endl << endl;

			input << "g part_" << QString::number(part) << endl << endl;

			int Fnum = 0;
			for (SurfaceMeshModel::Face_iterator fit = meshes[part]->faces_begin(); fit != meshes[part]->faces_end(); ++fit)
			{
				SurfaceMeshModel::Vertex_around_face_circulator fvit = meshes[part]->vertices(fit), fvend = fvit;
				input << "f ";
				do
				{
					input << ((SurfaceMeshModel::Vertex)fvit).idx() + 1 + totalNum << "//" << ((SurfaceMeshModel::Vertex)fvit).idx() + 1 + totalNum << " ";
				} while (++fvit != fvend);
				input << endl;
				Fnum++;
			}
			input << "# " << Fnum << " faces" << endl;
			totalNum += Vnum;
		}
		file.close();
	}
};