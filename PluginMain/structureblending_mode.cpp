#include <QMouseEvent>
#include "SurfaceMeshPlugins.h"
#include "ModePluginDockWidget.h"
#include "structureblending_mode.h"
#include "StarlabDrawArea.h"

#include <QFileDialog>

#include "SegMeshLoader.h"
#include "UtilityGlobal.h"

structureblending_mode::structureblending_mode()
{
    this->widget = NULL;
	Current_Single_Object = NULL;
}

void structureblending_mode::create()
{
    if(!widget)
    {
        ModePluginDockWidget * dockwidget = new ModePluginDockWidget("Structure Blending", mainWindow());
        widget = new stbl_widget(this);
        dockwidget->setWidget(widget);
        mainWindow()->addDockWidget(Qt::RightDockWidgetArea,dockwidget);
    }

    update();
}

void structureblending_mode::update()
{

}

bool structureblending_mode::wheelEvent(QWheelEvent * e)
{
    return false;
}

bool structureblending_mode::mouseMoveEvent(QMouseEvent * e)
{
    return false;
}

bool structureblending_mode::keyPressEvent(QKeyEvent *)
{
    return false;
}

void structureblending_mode::decorate()
{
	
}

void structureblending_mode::drawWithNames()
{
    double vt = 0;

	qglviewer::Vec viewDir = drawArea()->camera()->viewDirection().unit();
    Vector3 cameraNormal(viewDir[0],viewDir[1],viewDir[2]);

    foreach(Face f, mesh()->faces())
    {
        if(dot(fnormals[f], cameraNormal) > vt) continue;

        glPushName(f.idx());
        glBegin(GL_POLYGON);
        Surface_mesh::Vertex_around_face_circulator vit, vend;
        vit = vend = mesh()->vertices(f);
        do{ glVertex3dv(points[vit].data()); } while(++vit != vend);
        glEnd();
        glPopName();
    }
}

void structureblending_mode::LoadSingleObject()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("OBJ Files (*.obj);; OFF Files (*.off)"));

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		Current_Single_Object = new SurfaceMeshModel(filenames[0]);
		Current_Single_Object->read(filenames[0].toStdString());
		Current_Single_Object->updateBoundingBox();
		Current_Single_Object->update_face_normals();
		Current_Single_Object->update_vertex_normals();
		document()->clear();
		document()->addModel(Current_Single_Object);

		drawArea()->updateGL();
	}
}

void structureblending_mode::LoadSingleObjectSplit()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("OBJ Files (*.obj);; OFF Files (*.off)"));

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		Current_Single_Object = new SurfaceMeshModel(filenames[0]);
		Current_Single_Object->read(filenames[0].toStdString());
		Current_Single_Object->updateBoundingBox();
		Current_Single_Object->update_face_normals();
		Current_Single_Object->update_vertex_normals();

		document()->clear();
		document()->addModel(Current_Single_Object);

		Current_parts = connectedPieces(Current_Single_Object);

		drawArea()->updateGL();
	}
}

void structureblending_mode::LoadListSplit()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("CSV Files (*.csv)"));

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		SplitListShapes(filenames[0]);
	}
}