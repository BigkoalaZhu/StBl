#include <QMouseEvent>
#include "SurfaceMeshPlugins.h"
#include "ModePluginDockWidget.h"
#include "structureblending_mode.h"
#include "StarlabDrawArea.h"

#include <QFileDialog>

#include "SegMeshLoader.h"
#include "UtilityGlobal.h"

#include "PlausibilityDistance.h"

#include "nurbs_global.h"
#include "BoundaryFitting.h"
#include "writeOBJ.h"
#include "CorrespondenceEvaluate.h"

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
		pairFile = filenames[0];
	}
}

void structureblending_mode::SourceSeleclPart(QModelIndex index)
{
	corrfinder->DrawSpecificPart(index.row(), 0);
	drawArea()->updateGL();
}

void structureblending_mode::TargetSeleclPart(QModelIndex index)
{
	corrfinder->DrawSpecificPart(index.row(), 1);
	drawArea()->updateGL();
}

void structureblending_mode::outputGraphXML(QString filename, InitialStructureGraph graph)
{
	QDir path_dir;
	QString filepath = pairFile;
	filepath.chop(6);
	if (!path_dir.exists(filepath))
	{
		path_dir.mkpath(filepath);
	}
	if (!path_dir.exists(filepath + "/" + filename))
	{
		path_dir.mkpath(filepath + "/" + filename);
	}

	filepath += "/" + filename;
	QFile file(filepath + ".xml");
	file.open(QIODevice::ReadWrite | QIODevice::Text);
	QTextStream out(&file);
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << "\n";
	out << "<document>" << "\n";
	for (int i = 0; i < graph.meshes.size(); i++)
	{
		out << "<node>" << "\n";
		out << "\t<id>Part" + QString::number(i) + "</id>" << "\n";
		if (graph.flatIndex[i] == 0)
			out << "\t<type>CURVE</type>" << "\n";
		else
			out << "\t<type>SHEET</type>" << "\n";
		out << "\t<mesh>" + filename + "/Part" + QString::number(i) + ".obj</mesh>" << "\n";
		writeOBJ::wirte(graph.meshes[i], filepath + "/Part" + QString::number(i) + ".obj");
		out << "\n";
		out << "\t<controls>\n";
		if (graph.flatIndex[i] == 0)
			out << "\t\t<c>" + QString::number(graph.curves[i].size()) + "</c>\n";
		else
		{
			out << "\t\t<c>" + QString::number(graph.sheets[i].size()) + "</c>\n";
			out << "\t\t<c>" + QString::number(graph.sheets[i][0].size()) + "</c>\n";
		}
		out << "\t</controls>\n\n";

		if (graph.flatIndex[i] == 0)
		{
			for (int j = 0; j < graph.curves[i].size(); j++)
				out << "\t<point>" << graph.curves[i][j][0] << " " << graph.curves[i][j][1] << " " << graph.curves[i][j][2] << "</point>\n";
			out << "\t<weights>";
			for (int j = 0; j < graph.curves[i].size(); j++)
				out << "1 ";
			out << "</weights>\n";
		}
		else
		{
			for (int j = 0; j < graph.sheets[i].size(); j++)
				for (int k = 0; k < graph.sheets[i][j].size(); k++)
				{
					out << "\t<point>" << graph.sheets[i][j][k][0] << " " << graph.sheets[i][j][k][1] << " " << graph.sheets[i][j][k][2] << "</point>\n";
				}
			out << "\n\t<weights>";
			for (int j = 0; j < graph.sheets[i].size()*graph.sheets[i][0].size(); j++)
				out << "1 ";
			out << "</weights>\n";
		}
		out << "</node>" << "\n" << "\n";
	}
	for (int i = 0; i < graph.edges.size(); i++)
	{
		out << "<edge>" << "\n";
		out << "\t<id>Part" << graph.edges[i].first << ":Part" << graph.edges[i].second << "</id>" << "\n";
		out << "\t<type>POINT</type>" << "\n\n";
		out << "\t<n>Part" << graph.edges[i].first << "</n>" << "\n";
		out << "\t<n>Part" << graph.edges[i].second << "</n>" << "\n";
		out << "\t<coord>\n";
		out << "\t\t<uv>0 0 0 0</uv>\n";
		out << "\t</coord>\n";
		out << "\t<coord>\n";
		out << "\t\t<uv>0 0 0 0</uv>\n";
		out << "\t</coord>\n";
		out << "</edge>" << "\n\n";
	}
	for (int i = 0; i < graph.groups.size(); i++)
	{
		out << "<group>" << "\n";
		for (int j = 0; j < graph.groups[i].size(); j++)
			out << "\t<n>Part" << graph.groups[i][j] << "</n>" << "\n";
		out << "</group>" << "\n\n";
	}
	out << "</document>" << "\n";
	file.close();

	Structure::Graph *Graph = new Structure::Graph(filepath + ".xml", 0);
	Graph->saveToFile(filepath + ".xml", false);
}

void structureblending_mode::GeneratePartSet()
{
//	CorrespondenceEvaluate ce("C:/Users/cza68/Documents/CodingWork/StructureBlending/StBl/PluginMain/testData/testpair/SourceShape.xml", "C:/Users/cza68/Documents/CodingWork/StructureBlending/StBl/PluginMain/testData/testpair/TargetShape.xml");
//	ce.initialGraph();
/*	QVector<QString> sourcec;
	QVector<QString> targetc;
	sourcec.push_back("Part3");
	sourcec.push_back("Part16");
	sourcec.push_back("Part17");
	sourcec.push_back("Part23");
	targetc.push_back("Part14");
	targetc.push_back("Part16");
	targetc.push_back("Part15");
	targetc.push_back("Part13");
	targetc.push_back("Part9");
	targetc.push_back("Part5");
	targetc.push_back("Part6");
	targetc.push_back("Part1");
	targetc.push_back("Part10");
	targetc.push_back("Part3");
	ce.assignCorr(sourcec, targetc);
	ce.generateInbetween(0.5);*/
	corrfinder->GeneratePartSet();
/*	for (int i = 0; i < corrfinder->SourceStructureGraph.meshes.size(); i++)
	{
		if (corrfinder->SourceStructureGraph.flatIndex[i] == 1)
		{
			SurfaceMeshModel * tmpMesh = new SurfaceMeshModel;
			tmpMesh = corrfinder->SourceStructureGraph.meshes[i]->clone();
			FilterPlugin * matPlugin = pluginManager()->getFilter("Skeleton | Voronoi based MAT");
			RichParameterSet * mat_params = new RichParameterSet;
			matPlugin->initParameters(mat_params);
			document()->addModel(tmpMesh);
			document()->setSelectedModel(tmpMesh);
			matPlugin->applyFilter(mat_params);
			for (int j = 0; j < 1; j++)
			{
				FilterPlugin * mcfPlugin = pluginManager()->getFilter("Skeleton | MCF Skeletonization");

				RichParameterSet * mcf_params = new RichParameterSet;
				mcfPlugin->initParameters(mcf_params);
				mcfPlugin->applyFilter(mcf_params);
			}
			
			std::vector<std::vector<Vector3d>> cpts = surfaceFit(tmpMesh).mCtrlPoint;
			corrfinder->SourceStructureGraph.sheets[i] = cpts;
			document()->deleteModel(tmpMesh);
		}
	}

	for (int i = 0; i < corrfinder->TargetStructureGraph.meshes.size(); i++)
	{
		if (corrfinder->TargetStructureGraph.flatIndex[i] == 1)
		{
			SurfaceMeshModel * tmpMesh = new SurfaceMeshModel;
			tmpMesh = corrfinder->TargetStructureGraph.meshes[i]->clone();
			FilterPlugin * matPlugin = pluginManager()->getFilter("Skeleton | Voronoi based MAT");
			RichParameterSet * mat_params = new RichParameterSet;
			matPlugin->initParameters(mat_params);
			document()->addModel(tmpMesh);
			document()->setSelectedModel(tmpMesh);
			matPlugin->applyFilter(mat_params);
			for (int j = 0; j < 1; j++)
			{
				FilterPlugin * mcfPlugin = pluginManager()->getFilter("Skeleton | MCF Skeletonization");

				RichParameterSet * mcf_params = new RichParameterSet;
				mcfPlugin->initParameters(mcf_params);
				mcfPlugin->applyFilter(mcf_params);
			}
			
			std::vector<std::vector<Vector3d>> cpts = surfaceFit(tmpMesh).mCtrlPoint;
			corrfinder->TargetStructureGraph.sheets[i] = cpts;
			document()->deleteModel(tmpMesh);
		}
	}*/

	outputGraphXML("SourceShape", corrfinder->SourceStructureGraph);
	outputGraphXML("TargetShape", corrfinder->TargetStructureGraph);

	QStringList SourcePartList, TargetPartList;
	for (int i = 0; i < corrfinder->SourceGraphGroups.size(); i++)
	{
		QString tmp = QString::number(i);
		SourcePartList += tmp;
	}

	for (int i = 0; i < corrfinder->TargetGraphGroups.size(); i++)
	{
		QString tmp = QString::number(i);
		TargetPartList += tmp;
	}

	QStringListModel * SourceModel;
	QStringListModel * TargetModel;

	SourceModel = new QStringListModel(SourcePartList);
	TargetModel = new QStringListModel(TargetPartList);

	widget->ui->SourcrPartSet->setModel(SourceModel);
	widget->ui->TargetPartSet->setModel(TargetModel);
	drawArea()->updateGL();
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

std::vector<Vertex> structureblending_mode::collectRings(SurfaceMeshModel * part, Vertex v, size_t min_nb)
{
	std::vector<Vertex> all;
	std::vector<Vertex> current_ring, next_ring;
	SurfaceMeshModel::Vertex_property<int> visited_map = part->vertex_property<int>("v:visit_map", -1);

	//initialize
	visited_map[v] = 0;
	current_ring.push_back(v);
	all.push_back(v);

	int i = 1;

	while ((all.size() < min_nb) && (current_ring.size() != 0)){
		// collect i-th ring
		std::vector<Vertex>::iterator it = current_ring.begin(), ite = current_ring.end();

		for (; it != ite; it++){
			// push neighbors of 
			SurfaceMeshModel::Halfedge_around_vertex_circulator hedgeb = part->halfedges(*it), hedgee = hedgeb;
			do{
				Vertex vj = part->to_vertex(hedgeb);

				if (visited_map[vj] == -1){
					visited_map[vj] = i;
					next_ring.push_back(vj);
					all.push_back(vj);
				}

				++hedgeb;
			} while (hedgeb != hedgee);
		}

		//next round must be launched from p_next_ring...
		current_ring = next_ring;
		next_ring.clear();

		i++;
	}

	//clean up
	part->remove_vertex_property(visited_map);

	return all;
}

NURBS::NURBSRectangled structureblending_mode::surfaceFit(SurfaceMeshModel * part)
{
	Surface_mesh::Vertex_property<Vector3> points = part->vertex_property<Vector3>("v:point");

	/// Pick a side by clustering normals

	// 1) Find edge with flat dihedral angle
	SurfaceMeshModel::Vertex_property<double> vals = part->vertex_property<double>("v:vals", 0);
	foreach(Vertex v, part->vertices()){
		double sum = 0.0;
		foreach(Vertex v, collectRings(part, v, 12)){
			foreach(Halfedge h, part->onering_hedges(v)){
				sum += abs(calc_dihedral_angle(part, h));
			}
		}
		vals[v] = sum;
	}

	double minSum = DBL_MAX;
	Vertex minVert;
	foreach(Vertex v, part->vertices()){
		if (vals[v] < minSum){
			minSum = vals[v];
			minVert = v;
		}
	}
	Halfedge startEdge = part->halfedge(minVert);

	// 2) Grow region by comparing difference of adjacent dihedral angles
	double angleThreshold = deg_to_rad(40.0);

	SurfaceMesh::Model::Vertex_property<bool> vvisited = part->add_vertex_property<bool>("v:visited", false);

	QStack<SurfaceMesh::Model::Vertex> to_visit;
	to_visit.push(part->to_vertex(startEdge));

	while (!to_visit.empty())
	{
		Vertex cur_v = to_visit.pop();
		if (vvisited[cur_v]) continue;
		vvisited[cur_v] = true;

		// Sum of angles around
		double sumAngles = 0.0;
		foreach(Halfedge hj, part->onering_hedges(cur_v)){
			sumAngles += abs(calc_dihedral_angle(part, hj));
		}

		foreach(Halfedge hj, part->onering_hedges(cur_v))
		{
			Vertex vj = part->to_vertex(hj);
			if (sumAngles < angleThreshold)
				to_visit.push(vj);
			else
				vvisited[vj];
		}
	}

	// Get filtered inner vertices of selected side
	int shrink_count = 2;
	std::set<Vertex> inner;
	std::set<Vertex> border;
	for (int i = 0; i < shrink_count; i++)
	{
		std::set<Vertex> all_points;
		foreach(Vertex v, part->vertices())
			if (vvisited[v]) all_points.insert(v);

		border.clear();
		foreach(Vertex v, part->vertices()){
			if (vvisited[v]){
				foreach(Halfedge hj, part->onering_hedges(v)){
					Vertex vj = part->to_vertex(hj);
					if (!vvisited[vj])
						border.insert(vj);
				}
			}
		}

		inner.clear();
		std::set_difference(all_points.begin(), all_points.end(), border.begin(), border.end(),
			std::inserter(inner, inner.end()));

		// Shrink one level
		foreach(Vertex vv, border){
			foreach(Halfedge hj, part->onering_hedges(vv))
				vvisited[part->to_vertex(hj)] = false;
		}
	}

	SurfaceMesh::Model * submesh = NULL;

	bool isOpen = false;
	foreach(Vertex v, part->vertices()){
		if (part->is_boundary(v)){
			isOpen = true;
			break;
		}
	}

	if (!isOpen)
	{
		// Collect inner faces
		std::set<Face> innerFaces;
		std::set<Vertex> inFacesVerts;
		foreach(Vertex v, inner)
		{
			foreach(Halfedge hj, part->onering_hedges(v)){
				Face f = part->face(hj);
				innerFaces.insert(f);
				Surface_mesh::Vertex_around_face_circulator vit = part->vertices(f), vend = vit;
				do{ inFacesVerts.insert(Vertex(vit)); } while (++vit != vend);
			}
		}

		// Create sub-mesh
		submesh = new SurfaceMesh::Model("SideFlat.obj", "SideFlat");

		// Add vertices
		std::map<Vertex, Vertex> vmap;
		foreach(Vertex v, inFacesVerts){
			vmap[v] = Vertex(vmap.size());
			submesh->add_vertex(points[v]);
		}

		// Add faces
		foreach(Face f, innerFaces){
			std::vector<Vertex> verts;
			Surface_mesh::Vertex_around_face_circulator vit = part->vertices(f), vend = vit;
			do{ verts.push_back(Vertex(vit)); } while (++vit != vend);
			submesh->add_triangle(vmap[verts[0]], vmap[verts[1]], vmap[verts[2]]);
		}
	}
	else
	{
		submesh = part;
	}

	{
		//ModifiedButterfly subdiv;
		//subdiv.subdivide((*(Surface_mesh*)submesh),1);
	}


	submesh->isVisible = false;

	Vector3VertexProperty sub_points = submesh->vertex_property<Vector3>("v:point");

		// Smoothing
/*	{
	int numIteration = 3;
	bool protectBorders = true;

	Surface_mesh::Vertex_property<Point> newPositions = submesh->vertex_property<Point>("v:new_point", Vector3(0, 0, 0));
	Surface_mesh::Vertex_around_vertex_circulator vvit, vvend;

	// This method uses the basic equal weights Laplacian operator
	for (int iteration = 0; iteration < numIteration; iteration++)
	{
	Surface_mesh::Vertex_iterator vit, vend = submesh->vertices_end();

	// Original positions, for boundary
	for (vit = submesh->vertices_begin(); vit != vend; ++vit)
	newPositions[vit] = sub_points[vit];

	// Compute Laplacian
	for (vit = submesh->vertices_begin(); vit != vend; ++vit)
	{
	if (!protectBorders || (protectBorders && !submesh->is_boundary(vit)))
	{
	newPositions[vit] = Point(0, 0, 0);

	// Sum up neighbors
	vvit = vvend = submesh->vertices(vit);
	do{ newPositions[vit] += sub_points[vvit]; } while (++vvit != vvend);

	// Average it
	newPositions[vit] /= submesh->valence(vit);
	}
	}

	// Set vertices to final position
	for (vit = submesh->vertices_begin(); vit != vend; ++vit)
	sub_points[vit] = newPositions[vit];
	}

	submesh->remove_vertex_property(newPositions);
	}*/

	/// ==================
	// Fit rectangle

	BoundaryFitting bf((SurfaceMeshModel*)submesh, 6);


	Array2D_Vector3 cp = bf.lines;

	if (!cp.size()) return NURBS::NURBSRectangled::createSheet(Vector3d(0.0, 0.0, 0.0), Vector3d(0.01, 0.01, 0.01));

	Array2D_Real cw(cp.size(), Array1D_Real(cp.front().size(), 1.0));
	int degree = 3;
	return NURBS::NURBSRectangled(cp, cw, degree, degree, false, false, true, true);
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