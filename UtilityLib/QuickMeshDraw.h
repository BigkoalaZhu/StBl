#pragma once

#include <qgl.h>
#include "SurfaceMeshModel.h"
#include "RenderObjectExt.h"
using namespace SurfaceMesh;

struct QuickMeshDraw{

	static void drawMeshSolid( SurfaceMeshModel * mesh, QColor c = QColor(255,255,255,255), Vector3 translation = Vector3(0,0,0) )
	{
		if(!mesh) return;

        if(!mesh->has_face_property<Vector3>("f:normal")) mesh->update_face_normals();

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LIGHTING);

		glColorQt(c);

		Surface_mesh::Vertex_property<Vector3> points = mesh->vertex_property<Vector3>("v:point");
		Surface_mesh::Face_property<Vector3> fnormals = mesh->face_property<Vector3>("f:normal");

		Surface_mesh::Face_iterator fit, fend = mesh->faces_end();
		Surface_mesh::Vertex_around_face_circulator fvit, fvend;

		glPushMatrix();
		glTranslated(translation[0], translation[1], translation[2]);

		glBegin(GL_TRIANGLES);
		for (fit=mesh->faces_begin(); fit!=fend; ++fit){
			glNormal3dv( fnormals[fit].data() );
			fvit = fvend = mesh->vertices(fit);
			do{ glVertex3dv( points[fvit].data() ); } while (++fvit != fvend);
		}
		glEnd();

		glPopMatrix();
	}

	static void drawPartMeshSolid(SurfaceMeshModel * mesh, Vector3 translation = Vector3(0, 0, 0))
	{
		if (!mesh) return;

		if (!mesh->has_face_property<Vector3>("f:normal")) mesh->update_face_normals();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LIGHTING);

		Surface_mesh::Vertex_property<Vector3> points = mesh->vertex_property<Vector3>("v:point");
		Surface_mesh::Face_property<Vector3> fnormals = mesh->face_property<Vector3>("f:normal");
		Surface_mesh::Face_property<QColor> fcolors = mesh->face_property<QColor>("f:partcolor");

		Surface_mesh::Face_iterator fit, fend = mesh->faces_end();
		Surface_mesh::Vertex_around_face_circulator fvit, fvend;

		glPushMatrix();
		glTranslated(translation[0], translation[1], translation[2]);

		glBegin(GL_TRIANGLES);
		for (fit = mesh->faces_begin(); fit != fend; ++fit){
			glNormal3dv(fnormals[fit].data());
			glColorQt(fcolors[fit]);
			fvit = fvend = mesh->vertices(fit);
			do{ glVertex3dv(points[fvit].data()); } while (++fvit != fvend);
		}
		glEnd();

		glPopMatrix();
	}

	static void drawMeshWireFrame( SurfaceMeshModel * mesh )
	{
		if(!mesh) return;

		Surface_mesh::Face_iterator fit, fend = mesh->faces_end();
		Surface_mesh::Vertex_around_face_circulator fvit, fvend;
		Surface_mesh::Vertex_property<Vector3> points = mesh->vertex_property<Vector3>("v:point");
		Surface_mesh::Face_property<Vector3> fnormals = mesh->face_property<Vector3>("f:normal");

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4d(0,1,1, 0.25);
		glLineWidth(1.0f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		for (fit=mesh->faces_begin(); fit!=fend; ++fit){
			glBegin(GL_POLYGON);
			glNormal3dv( fnormals[fit].data() );
			fvit = fvend = mesh->vertices(fit);
			do{ glVertex3dv( points[fvit].data() ); } while (++fvit != fvend);
			glEnd();
		}
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		glDisable(GL_CULL_FACE);
	}

	static void drawMeshName( SurfaceMeshModel * mesh, int name = 0 )
	{
		glPushName( name );

		Surface_mesh::Vertex_property<Vector3> points = mesh->vertex_property<Vector3>("v:point");
		Surface_mesh::Face_iterator fit, fend = mesh->faces_end();
		Surface_mesh::Vertex_around_face_circulator fvit, fvend;

		glBegin(GL_TRIANGLES);
		for (fit=mesh->faces_begin(); fit!=fend; ++fit){
			fvit = fvend = mesh->vertices(fit);
			do{ glVertex3dv( points[fvit].data() ); } while (++fvit != fvend);
		}
		glEnd();

		glPopName();
	}
};
