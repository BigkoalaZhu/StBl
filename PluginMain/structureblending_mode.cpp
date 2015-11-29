#include <QMouseEvent>
#include "SurfaceMeshPlugins.h"
#include "ModePluginDockWidget.h"
#include "structureblending_mode.h"
#include "StarlabDrawArea.h"

#include <QFileDialog>

#include "SegMeshLoader.h"
#include "UtilityGlobal.h"

#include "PlausibilityDistance.h"

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

		CameraPath = "..\\UtilityLib\\cameras";
		hasPart = false;
		hasInbetween = false;
		corrfinder = NULL;
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
	if (corrfinder == NULL)
		return;
	corrfinder->DrawPartShape();
}

void structureblending_mode::drawWithNames()
{
	if (corrfinder == NULL)
		return;
    double vt = 0;

	qglviewer::Vec viewDir = drawArea()->camera()->viewDirection().unit();
    Vector3 cameraNormal(viewDir[0],viewDir[1],viewDir[2]);

    foreach(Face f, corrfinder->getSourceShape()->faces())
    {
        if(dot(fnormals[f], cameraNormal) > vt) continue;

        glPushName(f.idx());
        glBegin(GL_POLYGON);
        Surface_mesh::Vertex_around_face_circulator vit, vend;
		vit = vend = corrfinder->getSourceShape()->vertices(f);
		points = corrfinder->getSourceShape()->vertex_property<Vector3>("v:point");
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

void structureblending_mode::LoadShapePair()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Pair Files (*.spair)"));

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		corrfinder = new CorrFinder;
		corrfinder->LoadPairFile(filenames[0], hasPart, hasInbetween);
	}
}

void structureblending_mode::GeneratePartSet()
{
	corrfinder->GeneratePartSet();
}

void structureblending_mode::HasPartChange(int check)
{
	hasPart = check;
}

void structureblending_mode::HasInbetweenChange(int check)
{
	hasInbetween = check;
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

void structureblending_mode::Filtering()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::DirectoryOnly);

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		////////////////////////////
		QString PathName = filenames[0];
		PathName.append(".csv");
		QVector<ShapeNetModelInfo> ShapeList;
		ShapeNetModelInfo tmp;
		QString path = filenames[0];
		path.append("/");

		QFile file(PathName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) return;
		QTextStream in(&file);
		QString line = in.readLine();
		line = in.readLine();
		while (!line.isNull()) {
			QVector<QString> wholeline;
			wholeline.resize(6);
			int index = 0;
			int head = 0;
			bool status = true;
			for (int i = 0; i < line.length(); i++)
			{
				if (line.at(i) == ',' && status)
				{
					wholeline[index] = line.mid(head, i - head);
					head = i + 1;
					index++;
				}
				else if (line.at(i) == '\"')
					status = !status;
			}

			tmp.FileDescriptor = wholeline[2];
			tmp.FileDescriptor.remove("\"");
			tmp.FileLocation = wholeline[0];
			tmp.FileLocation.remove("3dw.");
			tmp.FileLocation.prepend(path);
			wholeline[3].remove("\"");
			wholeline[4].remove("\"");
			tmp.FrontDirection = QStringToVector3d(wholeline[4]);
			tmp.UpDirection = QStringToVector3d(wholeline[3]);
			ShapeList.push_back(tmp);
			line = in.readLine();
		}
		file.close();
		//////////////////////////////////////
		QVector<int> Names;
		for (int i = 0; i < ShapeList.size(); i++)
		{
			QString DirName = ShapeList[i].FileLocation;
			DirName.append("/parts");

			QDir SubDir(DirName);
			if (SubDir.count()>3 && SubDir.count() < 30)
				Names.push_back(i);
		}

		QString ListName = filenames[0];
		ListName.append("/validmodels.list");

		QMessageBox message(QMessageBox::Warning, "Warning", ListName, QMessageBox::Ok, NULL);
		message.exec();

		QFile outputList(ListName);
		if (!outputList.open(QIODevice::WriteOnly | QFile::Text)) return;
		QTextStream out(&outputList);
		for (int i = 0; i < Names.size(); i++)
		{
			out << ShapeList[Names[i]].FileLocation << ";;";
			out << ShapeList[Names[i]].FileDescriptor << ";;";
			out << ShapeList[Names[i]].FrontDirection[0] << "\\," << ShapeList[Names[i]].FrontDirection[1] << "\\," << ShapeList[Names[i]].FrontDirection[2] << ";;";;
			out << ShapeList[Names[i]].UpDirection[0] << "\\," << ShapeList[Names[i]].UpDirection[1] << "\\," << ShapeList[Names[i]].UpDirection[2] << ";;";;
			out << endl;
		}
		outputList.close();
	}
}

void structureblending_mode::CameraPathChange(QString path)
{
	CameraPath = path;
}

void structureblending_mode::LoadSingleMesh()
{
/*	QFileDialog dialog(mainWindow());
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

		projectImage = new GenerateProjectedImage(Current_Single_Object, CameraPath);

		drawArea()->updateGL();
	}*/

	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::DirectoryOnly);

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();

//		PlausibleSquenceSearching pss("distance.txt", 354);
//		pss.SetBiasAndTolerence(3000.0f, 0.3);
//		QVector<QVector<int>> paths = pss.FindPlausiblePath(182, 120);

//		QVector<ShapeNetModelInfo> ShapeList = ShapeNetFormate::LoadFolderSem(filenames[0], "03001627");

		PlausibilityDistance PD(filenames[0], 20);

//		PD.GenerateBiSHDescriptor();

//		double dd = PD.CalculatePairDistance(0, 343);

//		QMessageBox message(QMessageBox::Warning, "Warning", QString::number(dd), QMessageBox::Ok, NULL);
//		message.exec();


//		PD.GenerateBiSHDescriptor();

		PD.CalculatePairwiseDistance();

//		double dd1 = PD.CalculatePairDistance(507, 790);
//		double dd2 = PD.CalculatePairDistance(790, 2823);
//		double dd3 = PD.CalculatePairDistance(507, 2823);
//		double dd4 = PD.CalculatePairDistance(236, 240);

//		QMessageBox message(QMessageBox::Warning, "Warning", QString::number(dd1) + "," + QString::number(dd2) + "," + QString::number(dd3), QMessageBox::Ok, NULL);
//		message.exec();
	}
}

void structureblending_mode::CameraIndexChange(QString index)
{
	CameraIndex = index.toInt();
}

void structureblending_mode::GenerateSingleImage()
{
	projectImage->projectImage(CameraIndex, "test.bmp");
}

void structureblending_mode::LoadAList2GenerateImages()
{
	QFileDialog dialog(mainWindow());
	dialog.setDirectory(QDir::currentPath());
	dialog.setFileMode(QFileDialog::DirectoryOnly);

	if (dialog.exec())
	{
		QStringList filenames = dialog.selectedFiles();
		QVector<ShapeNetModelInfo> ShapeList = ShapeNetFormate::LoadFolder(filenames[0]);

//		EMDMorphing2D em(filenames[0], filenames[0]);
//		em.Initialization();
//		em.GetInbetween(0.5, "test.bmp");


		for (int i = 0; i < 1; i++)
		{
			QString filename = ShapeList[i].FileLocation + "/model.obj";
			QDir dir(ShapeList[i].FileLocation + "/ProjectedImages");

			if (!dir.exists())
				dir.mkdir(dir.absolutePath());
			else if (dir.count() == 202)
				continue;
			GenerateProjectedImage *pi = new GenerateProjectedImage(filename, CameraPath);

//			#pragma omp parallel for
			for (int j = 0; j < 20; j++)
				pi->projectImage(j, dir.absolutePath() + "/" + QString::number(j) + ".jpg",1);
		}
	}
}