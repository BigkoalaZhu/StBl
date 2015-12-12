#include "CorrFinder.h"
#include "Colormaps.h"
#include "QuickMeshDraw.h"
#include <qfile.h>
#include "OBB_Volume.h"
#include "UtilityGlobal.h"
#include "SegGraph.h"
#include "nurbs_global.h"
#include "BoundaryFitting.h"

#include "writeOBJ.h"
#include "RichParameterSet.h"

CorrFinder::CorrFinder()
{
	ColorMap = makeColorMapJet();
	ColorDifferent.resize(6);
	ColorDifferent[0] = Qt::GlobalColor::red;
	ColorDifferent[1] = Qt::GlobalColor::green;
	ColorDifferent[2] = Qt::GlobalColor::blue;
	ColorDifferent[3] = Qt::GlobalColor::yellow;
	ColorDifferent[4] = Qt::GlobalColor::cyan;
	ColorDifferent[5] = Qt::GlobalColor::magenta;
	threshouldGC = 4.0;
}


CorrFinder::~CorrFinder()
{
}

std::vector<Vertex> CorrFinder::collectRings(SurfaceMeshModel * part, Vertex v, size_t min_nb)
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

NURBS::NURBSRectangled CorrFinder::surfaceFit(SurfaceMeshModel * part)
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

/*	// Smoothing
	{
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

	BoundaryFitting bf((SurfaceMeshModel*)submesh);


	Array2D_Vector3 cp = bf.lines;

	if (!cp.size()) return NURBS::NURBSRectangled::createSheet(Vector3d(0.0, 0.0, 0.0), Vector3d(0.01, 0.01, 0.01));

	Array2D_Real cw(cp.size(), Array1D_Real(cp.front().size(), 1.0));
	int degree = 3;
	return NURBS::NURBSRectangled(cp, cw, degree, degree, false, false, true, true);
}

void CorrFinder::DrawPartShape()
{
	{
		QuickMeshDraw::drawPartMeshSolid(SourceShape, SurfaceMesh::Vector3(-0.5, 0, 0));
		QuickMeshDraw::drawPartMeshSolid(TargetShape, SurfaceMesh::Vector3(0.5, 0, 0));
	}
}

bool CorrFinder::LoadPairFile(QString filepath, bool hasPart, bool hasInbetween)
{
	QFile pairfile(filepath);
	pairfile_path = filepath;
	if (!pairfile.open(QIODevice::ReadOnly))
		return false;

	QString folder = filepath;
	folder.chop(folder.length() - folder.lastIndexOf("/") - 1);

	QTextStream in(&pairfile);
	QString str = in.readLine();
	QStringList strtmp = str.split(" ");
	sourceShape_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetShape_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	sourceIndex_path = folder + strtmp[1];

	str = in.readLine();
	strtmp = str.split(" ");
	targetIndex_path = folder + strtmp[1];

	if (hasPart)
	{
		str = in.readLine();
		strtmp = str.split(" ");
		SourceShapePartNum = strtmp[1].toInt();
		SourceShapePartIndex.resize(SourceShapePartNum);
		for (int i = 0; i < SourceShapePartNum; i++)
		{
			str = in.readLine();
			strtmp = str.split(" ");
			for (int j = 0; j < strtmp.size(); j++)
				SourceShapePartIndex[i].push_back(strtmp[j].toInt());
		}

		str = in.readLine();
		strtmp = str.split(" ");
		TargetShapePartNum = strtmp[1].toInt();
		TargetShapePartIndex.resize(TargetShapePartNum);
		for (int i = 0; i < TargetShapePartNum; i++)
		{
			str = in.readLine();
			strtmp = str.split(" ");
			for (int j = 0; j < strtmp.size(); j++)
				TargetShapePartIndex[i].push_back(strtmp[j].toInt());
		}
	}

	if (hasInbetween)
	{
		if (!hasPart)
		{
			str = in.readLine();
			str = in.readLine();
		}
		str = in.readLine();
		strtmp = str.split(" ");
		int InbetweenNum = strtmp[1].toInt();
		for (int i = 0; i < InbetweenNum; i++)
		{
			str = in.readLine();
			InbetweenShapes.push_back(str);
		}
	}
	pairfile.close();

	SourceShape = new SurfaceMeshModel(sourceShape_path);
	SourceShape->read(sourceShape_path.toStdString());
	SourceShape->updateBoundingBox();
	SourceShape->update_face_normals();
	SourceShape->update_vertex_normals();
	SourceShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));
	SourceShape->add_face_property("f:seg", QVector<int>());

	TargetShape = new SurfaceMeshModel(targetShape_path);
	TargetShape->read(targetShape_path.toStdString());
	TargetShape->updateBoundingBox();
	TargetShape->update_face_normals();
	TargetShape->update_vertex_normals();
	TargetShape->add_face_property("f:partcolor", QColor(255, 255, 255, 255));
	TargetShape->add_face_property("f:seg", QVector<int>());

	SourceShapeSegmentIndex.resize(SourceShape->faces_size());
	SourceShapePartIndex.resize(SourceShape->faces_size());

	TargetShapeSegmentIndex.resize(TargetShape->faces_size());
	TargetShapePartIndex.resize(TargetShape->faces_size());

	if (!LoadParialPartFile())
		return false;

	FindSegAdjacencyMatrix(1);

	filterJoints(0);
	filterJoints(1);

	ApplySegmentColor();
	return true;
}

void CorrFinder::filterJoints(int SorT)
{
	int SegNum;
	QVector<int> SegmentJointIndex;
	QVector<double> SegmentJointGC;
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegNum = SourceShapeSegmentNum;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentJointGC = SourceShapeSegmentGC;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegNum = TargetShapeSegmentNum;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentJointGC = TargetShapeSegmentGC;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	for (int i = 0; i < SegNum; i++)
		if (SegmentJointGC[i] > threshouldGC && SegAdjacencyMatrix.row(i).sum() > 3)
			SegmentJointIndex[i] = -1;

	if (SorT == 0)
	{
		SourceShapeSegmentJointIndex = SegmentJointIndex;
	}
	else
	{
		TargetShapeSegmentJointIndex = SegmentJointIndex;
	}

}

void CorrFinder::ApplySegmentColor(int sindex)
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();

	double eachInternal = 1.0f / sourceVaildSegNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < SourceShapeSegmentNum; i++)
				if (SourceShapeSegmentIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (SourceShapeSegmentIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		if (SourceShapeSegmentJointIndex[selected] == -1)
		{
			index++;
			continue;
		}
		fscolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();
	
	eachInternal = 1.0f / targetVaildSegNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < TargetShapeSegmentNum; i++)
				if (TargetShapeSegmentIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (TargetShapeSegmentIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		if (TargetShapeSegmentJointIndex[selected] == -1)
		{
			index++;
			continue;
		}
		ftcolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}
}

void CorrFinder::FindSegAdjacencyMatrix(int ignoreJoint)
{
	Surface_mesh::Face_around_vertex_circulator fit, fend;
	Surface_mesh::Vertex_iterator vit;
	Surface_mesh::Face_property<QVector<int>> fsseg = SourceShape->face_property<QVector<int>>("f:seg");
	Surface_mesh::Face_property<QVector<int>> ftseg = TargetShape->face_property<QVector<int>>("f:seg");

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

	for (vit = SourceShape->vertices_begin(); vit != SourceShape->vertices_end(); ++vit)
	{
		QVector<int> segs;
		fit = fend = SourceShape->faces(vit);
		do{
			for (int i = 0; i < fsseg[fit].size(); i++)
				segs.push_back(fsseg[fit][i]);
		} while (++fit != fend);
		for (int i = 0; i < segs.size(); i++)
		{
			for (int j = i + 1; j < segs.size(); j++)
			{
				if (segs[i] != segs[j])
				{
					SourceSegAdjacencyMatrix(segs[i], segs[j]) += 1;
					SourceSegAdjacencyMatrix(segs[j], segs[i]) += 1;
				}		
			}
		}
	}

	for (int i = 0; i < SourceSegAdjacencyMatrix.rows(); i++)
	{
		for (int j = 0; j < SourceSegAdjacencyMatrix.cols(); j++)
		{
			if (i == j)
				continue;
			if (SourceSegAdjacencyMatrix(i, j) > 5)
				SourceSegAdjacencyMatrix(i, j) = 1;
		}
	}

	if (ignoreJoint == 0)
	{
		for (int i = 0; i < SourceShapeSegmentNum; i++)
		{
			QVector<int> segs;
			if (SourceShapeSegmentJointIndex[i] == 1)
				continue;
			for (int j = 0; j < SourceShapeSegmentNum; j++)
			{
				if (i == j || SourceShapeSegmentJointIndex[i] == 1)
					continue;
				if (SourceSegAdjacencyMatrix(i, j) == 1)
					segs.push_back(j);
			}
			for (int k = 0; k < segs.size(); k++)
			{
				for (int j = k + 1; j < segs.size(); j++)
				{
					if (segs[k] != segs[j])
					{
						SourceSegAdjacencyMatrix(segs[k], segs[j]) = 1;
						SourceSegAdjacencyMatrix(segs[j], segs[k]) = 1;
					}
				}
			}
		}
	}
	

	for (vit = TargetShape->vertices_begin(); vit != TargetShape->vertices_end(); ++vit)
	{
		QVector<int> segs;
		fit = fend = TargetShape->faces(vit);
		do{ 
			for (int i = 0; i < ftseg[fit].size(); i++)
				segs.push_back(ftseg[fit][i]);
		} while (++fit != fend);
		for (int i = 0; i < segs.size(); i++)
		{
			for (int j = i + 1; j < segs.size(); j++)
			{
				if (segs[i] != segs[j])
				{
					TargetSegAdjacencyMatrix(segs[i], segs[j]) += 1;
					TargetSegAdjacencyMatrix(segs[j], segs[i]) += 1;
				}		
			}
		}
	}

	for (int i = 0; i < TargetSegAdjacencyMatrix.rows(); i++)
	{
		for (int j = 0; j < TargetSegAdjacencyMatrix.cols(); j++)
		{
			if (i == j)
				continue;
			if (TargetSegAdjacencyMatrix(i, j) > 5)
				TargetSegAdjacencyMatrix(i, j) = 1;
		}
	}

	if (ignoreJoint == 0)
	{
		for (int i = 0; i < TargetShapeSegmentNum; i++)
		{
			QVector<int> segs;
			if (TargetShapeSegmentJointIndex[i] == 1)
				continue;
			for (int j = 0; j < TargetShapeSegmentNum; j++)
			{
				if (i == j || TargetShapeSegmentJointIndex[i] == 1)
					continue;
				if (TargetSegAdjacencyMatrix(i, j) == 1)
					segs.push_back(j);
			}
			for (int k = 0; k < segs.size(); k++)
			{
				for (int j = k + 1; j < segs.size(); j++)
				{
					if (segs[k] != segs[j])
					{
						TargetSegAdjacencyMatrix(segs[k], segs[j]) = 1;
						TargetSegAdjacencyMatrix(segs[j], segs[k]) = 1;
					}
				}
			}
		}
	}

}

void CorrFinder::GetSegFaceNum()
{
	SourceSegFaceNum.resize(SourceShapeSegmentNum);
	TargetSegFaceNum.resize(TargetShapeSegmentNum);
	SourceSegFaceNum.fill(0);
	TargetSegFaceNum.fill(0);

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		for each (int flag in SourceShapeSegmentIndex[i])
		{
			SourceSegFaceNum[i] += flag;
		}
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		for each (int flag in TargetShapeSegmentIndex[i])
		{
			TargetSegFaceNum[i] += flag;
		}
	}
}

void CorrFinder::GenerateSegMeshes(int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");
	QVector<SurfaceMeshModel *> SegmentMeshes;
	SegmentMeshes.resize(SegNum);

	for (int i = 0; i < SegNum; i++)
	{
		SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
		piece->reserve(uint(SegFaceNum[i] * 3), uint(SegFaceNum[i] * 6), uint(SegFaceNum[i]));
		for (auto v : proessShape->vertices())
		{
			piece->add_vertex(points[v]);
		}
		SegmentMeshes[i] = piece;
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1)
				SegmentMeshes[i]->add_face(face);
		findex++;
	}

	for (int i = 0; i < SegNum; i++)
	{
		for (auto v : SegmentMeshes[i]->vertices()) if (SegmentMeshes[i]->is_isolated(v)) SegmentMeshes[i]->remove_vertex(v);
		SegmentMeshes[i]->garbage_collection();
		SegmentMeshes[i]->updateBoundingBox();
		closeHoles(SegmentMeshes[i]);
		SegmentMeshes[i]->update_face_normals();
		SegmentMeshes[i]->update_vertex_normals();
	}
	if (SorT == 0)
		SourceShapeSegment = SegmentMeshes;
	else
		TargetShapeSegment = SegmentMeshes;
}

void CorrFinder::FlatSegMerge(double threshold, int SorT, QVector<int> &flat)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> ShapeSegmentJointIndex;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		ShapeSegmentJointIndex = SourceShapeSegmentJointIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		ShapeSegmentJointIndex = TargetShapeSegmentJointIndex;
	}
	
	Surface_mesh::Vertex_property<Surface_mesh::Vector3> points = proessShape->vertex_property<Surface_mesh::Vector3>("v:point");
	Surface_mesh::Face_iterator fit, fend;
	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	std::vector<std::vector<Vector3d>> SegPoints;
	QVector<int> SegFlat;

	SegFlat.resize(SegNum);
	SegPoints.resize(SegNum);
	fend = proessShape->faces_end();
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		QVector<int> flag;
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1)
				flag.push_back(i);
		fvit = fvend = proessShape->vertices(fit);
		do{
			for (int i = 0; i < flag.size(); i++)
				SegPoints[flag[i]].push_back(points[fvit]);
		} while (++fvit != fvend);
		findex++;
	}
	for (int i = 0; i < SegNum; i++)
	{
		OBB_Volume obb(SegPoints[i]);
		Vector3d extants = obb.extents();

		double maxc = extants.maxCoeff();
		double minc = extants.minCoeff();
		double middlec;
		for (int j = 0; j < 3; j++)
		{
			if (extants[j] != maxc&&extants[j] != minc)
			{
				middlec = extants[j];
				break;
			}
				
		}

		double oth = middlec / minc;
		if (oth > threshold)
			SegFlat[i] = 1;
		else
			SegFlat[i] = 0;
	}
	
	for (int i = 0; i < SegNum; i++)
	{
		for (int j = i + 1; j < SegNum; j++)
		{
			if (SegAdjacencyMatrix(i, j) == 0 || SegFlat[i] + SegFlat[j] == 0)
				continue;
/*			findex = 0;
			std::vector<Vector3d> tmpPoints;
			for (fit = proessShape->faces_begin(); fit != fend; ++fit){
				if (SegmentIndex[i][findex] == 1 || SegmentIndex[j][findex] == 1)
				{
					fvit = fvend = proessShape->vertices(fit);
					do{
						tmpPoints.push_back(points[fvit]);
					} while (++fvit != fvend);
				}
				findex++;
			}
			OBB_Volume obb(tmpPoints);
			Vector3d extants = obb.extents();

			double maxc = extants.maxCoeff();
			double minc = extants.minCoeff();
			double middlec;
			for (int k = 0; k < 3; k++)
			{
				if (extants[k] != maxc&&extants[k] != minc)
				{
					middlec = extants[k];
					break;
				}

			}
			double oth = middlec / minc;
			if (oth < threshold + 0.5)
				continue;*/

			if (!IsFlatMerge(i, j, SorT, threshold))
				continue;

			MergeTwoSegs(i, j, SorT);

			SegFlat[i] = 1;
			SegFlat.erase(SegFlat.begin() + j);
			if (SorT == 0)
			{
				SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
				ShapeSegmentJointIndex = SourceShapeSegmentJointIndex;
			}
			else
			{
				SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
				ShapeSegmentJointIndex = TargetShapeSegmentJointIndex;
			}
			j = i + 1;
			SegNum--;
		}
	}

	flat = SegFlat;
	ApplySegmentColor();
}

bool CorrFinder::IsFlatMerge(int indexA, int indexB, int SorT, double threshold)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> ShapeSegmentJointIndex;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		ShapeSegmentJointIndex = SourceShapeSegmentJointIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		ShapeSegmentJointIndex = TargetShapeSegmentJointIndex;
	}

	Surface_mesh::Vertex_property<Surface_mesh::Vector3> points = proessShape->vertex_property<Surface_mesh::Vector3>("v:point");
	Surface_mesh::Face_iterator fit, fend;
	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	std::vector<std::vector<Vector3d>> SegPoints;
	std::vector<Vector3d> SegPointsTotal;
	
	SegPoints.resize(2);
	fend = proessShape->faces_end();
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		QVector<int> flag;
		for (int i = 0; i < SegNum; i++)
			if (SegmentIndex[i][findex] == 1 && (i == indexA || i == indexB))
				flag.push_back(i);
		fvit = fvend = proessShape->vertices(fit);
		do{
			for (int i = 0; i < flag.size(); i++)
			{
				if (flag[i] == indexA)
				{
					SegPoints[0].push_back(points[fvit]);
					SegPointsTotal.push_back(points[fvit]);
				}
				else if (flag[i] == indexB)
				{
					SegPoints[1].push_back(points[fvit]);
					SegPointsTotal.push_back(points[fvit]);
				}
					
			}
		} while (++fvit != fvend);
		findex++;
	}

	OBB_Volume obbTotal(SegPointsTotal);
	OBB_Volume obbA(SegPoints[0]);
	OBB_Volume obbB(SegPoints[1]);

	Eigen::Vector3d extantsA = obbA.extents();
	Eigen::Vector3d extantsB = obbB.extents();
	Eigen::Vector3d extantsTotal = obbTotal.extents();

	int totalmiddle = 0;
	for (int i = 0; i < 3; i++)
	{
		if (extantsTotal[i] != extantsTotal.maxCoeff() && extantsTotal[i] != extantsTotal.minCoeff())
		{
			totalmiddle = i;
			break;
		}
	}

	if (extantsTotal[totalmiddle] / extantsTotal.minCoeff() < threshold)
		return false;

	if (ShapeSegmentJointIndex[indexA] == -1 || ShapeSegmentJointIndex[indexB] == -1)
		return true;

	double vAB = extantsA[0] * extantsA[1] * extantsA[2] + extantsB[0] * extantsB[1] * extantsB[2];
	double vtotal = extantsTotal[0] * extantsTotal[1] * extantsTotal[2];

	if (vtotal / vAB > 1)
		return false;
	else
		return true;

	int minA = 0, minB = 0, minTotal = 0;
	for (int i = 1; i < 3; i++)
	{
		if (extantsA[i] < extantsA[minA])
			minA = i;
		if (extantsB[i] < extantsB[minB])
			minB = i;
		if (extantsTotal[i] < extantsTotal[minTotal])
			minTotal = i;
	}

	if (abs(extantsA[minA] - extantsB[minB]) / std::min(extantsA[minA], extantsB[minB]) > 1)
		return false;
	
	std::vector<Vector3d> axisA = obbA.axis();
	std::vector<Vector3d> axisB = obbB.axis();

	axisA[minA].normalize();
	axisB[minB].normalize();

	if (abs(axisA[minA].dot(axisB[minB])) < 0.8)
		return false;

	Eigen::Vector3d centerA = obbA.center();
	Eigen::Vector3d centerB = obbB.center();

	Eigen::Vector3d meanAxis;
	
	if (axisA[minA].dot(axisB[minB]) < 0) 
		meanAxis = (axisA[minA] - axisB[minB]) / 2;
	else
		meanAxis = (axisA[minA] + axisB[minB]) / 2;

	centerA = meanAxis.dot(centerA) * meanAxis;
	centerB = meanAxis.dot(centerB) * meanAxis;

	if ((centerA - centerB).norm() > std::max(extantsA[minA], extantsB[minB]) && (centerA + centerB).norm() > std::max(extantsA[minA], extantsB[minB]))
		return false;

	return true;
}

void CorrFinder::MergeTwoSegs(int A, int B, int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;
	QVector<double> SegmentJointGC;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentAxis = SourceShapeSegmentAxis;
		SegmentAxisDirection = SourceShapeSegmentAxisDirection;
		SegmentJointGC = SourceShapeSegmentGC;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentAxis = TargetShapeSegmentAxis;
		SegmentAxisDirection = TargetShapeSegmentAxisDirection;
		SegmentJointGC = TargetShapeSegmentGC;
	}

	int minS = std::min(A, B);
	int maxS = std::max(A, B);

	for (int i = 0; i < proessShape->faces_size(); i++)
	{
		if (SegmentIndex[maxS][i] == 1)
			SegmentIndex[minS][i] = 1;
	}
	SegmentIndex.erase(SegmentIndex.begin() + maxS);

	for (int i = 0; i < SegNum; i++)
	{
		if (SegAdjacencyMatrix(maxS, i) == 1)
		{
			SegAdjacencyMatrix(minS, i) = 1;
			SegAdjacencyMatrix(i, minS) = 1;
		}
	}

	unsigned int numRows = SegAdjacencyMatrix.rows() - 1;
	unsigned int numCols = SegAdjacencyMatrix.cols();

	if (maxS < numRows)
		SegAdjacencyMatrix.block(maxS, 0, numRows - maxS, numCols) = SegAdjacencyMatrix.block(maxS + 1, 0, numRows - maxS, numCols);

	SegAdjacencyMatrix.conservativeResize(numRows, numCols);

	numRows = SegAdjacencyMatrix.rows();
	numCols = SegAdjacencyMatrix.cols() - 1;

	if (maxS < numCols)
		SegAdjacencyMatrix.block(0, maxS, numRows, numCols - maxS) = SegAdjacencyMatrix.block(0, maxS + 1, numRows, numCols - maxS);

	SegAdjacencyMatrix.conservativeResize(numRows, numCols);

	SegNum--;
	SegmentJointIndex.erase(SegmentJointIndex.begin() + maxS);
	SegmentJointGC.erase(SegmentJointGC.begin() + maxS);
	SegmentAxis.erase(SegmentAxis.begin() + maxS);
	SegmentAxisDirection.erase(SegmentAxisDirection.begin() + maxS);

	if (SorT == 0)
	{
		SourceShapeSegmentNum = SegNum;
		SourceShapeSegmentIndex = SegmentIndex;
		SourceSegAdjacencyMatrix = SegAdjacencyMatrix;
		SourceShapeSegmentJointIndex = SegmentJointIndex;
		SourceShapeSegmentGC = SegmentJointGC;
		SourceShapeSegmentAxis = SegmentAxis;
		SourceShapeSegmentAxisDirection = SegmentAxisDirection;
	}
	else
	{
		TargetShapeSegmentNum = SegNum;
		TargetShapeSegmentIndex = SegmentIndex;
		TargetSegAdjacencyMatrix = SegAdjacencyMatrix;
		TargetShapeSegmentJointIndex = SegmentJointIndex;
		TargetShapeSegmentGC = SegmentJointGC;
		TargetShapeSegmentAxis = SegmentAxis;
		TargetShapeSegmentAxisDirection = SegmentAxisDirection;
	}
	ApplySeg(SorT);
}

SurfaceMeshModel * CorrFinder::mergedSeg(QVector<int> indexes, int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");

	SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
	int totalFaceNum = 0;
	for each (int n in indexes)
		totalFaceNum += SegFaceNum[n];

	piece->reserve(uint(totalFaceNum * 3), uint(totalFaceNum * 6), uint(totalFaceNum));
	for (auto v : proessShape->vertices())
	{
		piece->add_vertex(points[v]);
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < indexes.size(); i++)
			if (SegmentIndex[indexes[i]][findex] == 1)
			{
				piece->add_face(face);
				break;
			}	
		findex++;
	}

	for (auto v : piece->vertices()) if (piece->is_isolated(v)) piece->remove_vertex(v);
	piece->garbage_collection();
	piece->updateBoundingBox();
	piece->update_face_normals();
	piece->update_vertex_normals();

	return piece;
}

bool CorrFinder::IsFlat(SegmentGroupFromGraph group, int SorT)
{
	for (int i = 0; i < group.labels.size(); i++)
	{
		for (int j = 0; j < group.labels[i].size(); j++)
		{
			if (SorT == 0)
				if (SourceShapeSegmentFlatIndex[group.labels[i][j]] == 1)
					return true;
			if (SorT == 1)
				if (TargetShapeSegmentFlatIndex[group.labels[i][j]] == 1)
					return true;
		}
	}
	return false;
}

void CorrFinder::MergeGraphSegToParts(int SorT)
{
	QVector<SegmentGroupFromGraph> GraphGroups;
	int SegNum;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;

	if (SorT == 0)
	{
		GraphGroups = SourceGraphGroups;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegNum = SourceShapeSegmentNum;
	}
	else
	{
		GraphGroups = TargetGraphGroups;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegNum = TargetShapeSegmentNum;
	}

	int candidatesNum = GraphGroups.size();
	for (int i = 0; i < GraphGroups.size(); i++)
	{
		if (i > candidatesNum)
			candidatesNum = GraphGroups.size();
		if (IsFlat(GraphGroups[i], SorT))
			continue;
		for (int j = 0; j < candidatesNum; j++)
		{
			if (i == j)
				continue;

			if (IsFlat(GraphGroups[j], SorT))
				continue;

			QVector<int> align;
			if (!IsAdjacented(GraphGroups[i], GraphGroups[j], SorT, align))
				continue;

			QVector<int> type;
			if (!IsSmoothConnected(GraphGroups[i], GraphGroups[j], align, type))
				continue;

			SegmentGroupFromGraph newgroup = MergeGroups(GraphGroups[i], GraphGroups[j], type, align);
			if (!IsExistedGroups(GraphGroups, newgroup))
				GraphGroups.push_back(newgroup);
		}
	}

	for (int i = 0; i < GraphGroups.size(); i++)
	{
		if (GraphGroups[i].labels.size() == 1)
			continue;
		QVector<QVector<int>> seqs;
		QVector<int> tmp;
		tmp.resize(GraphGroups[i].labels.size());
		Permutation(GraphGroups[i].labels.size(), tmp, seqs, 0);
		QVector<SegmentGroupFromGraph> tmpseggroups;
		for (int j = 0; j < GraphGroups[i].labels.size(); j++)
		{
			SegmentGroupFromGraph newgroup;
			newgroup.labels.resize(1);
			newgroup.SegmentAxis.resize(1);
			newgroup.SegmentAxisDirection.resize(1);
			newgroup.labels[0] = GraphGroups[i].labels[j];
			newgroup.SegmentAxis[0] = GraphGroups[i].SegmentAxis[j];
			newgroup.SegmentAxisDirection[0] = GraphGroups[i].SegmentAxisDirection[j];
			tmpseggroups.push_back(newgroup);
		}
		for (int j = 0; j < seqs.size(); j++)
		{
			int flag = 1;
			SegmentGroupFromGraph newgroup = tmpseggroups[seqs[j][0]];
			for (int k = 1; k < GraphGroups[i].labels.size(); k++)
			{
				QVector<int> align;
				if (!IsAdjacented(newgroup, tmpseggroups[seqs[j][k]], SorT, align))
					break;
				QVector<int> type;
				if (!IsSmoothConnected(newgroup, tmpseggroups[seqs[j][k]], align, type))
					break;
				newgroup = MergeGroups(newgroup, tmpseggroups[seqs[j][k]], type, align);
				flag++;
			}
			if (flag == GraphGroups[i].labels.size() && !IsExistedGroups(GraphGroups, newgroup))
				GraphGroups.push_back(newgroup);
		}
	}

	for (int i = 0; i < GraphGroups.size(); i++)
	{
		GraphGroups[i].joints.resize(GraphGroups[i].labels.size());
		GraphGroups[i].meshes.resize(GraphGroups[i].labels.size());
		for (int j = 0; j < GraphGroups[i].labels.size(); j++)
		{
			for (int k = 0; k < GraphGroups[i].labels[j].size(); k++)
			{
				if (!GraphGroups[i].allseg.contains(GraphGroups[i].labels[j][k]))
					GraphGroups[i].allseg.push_back(GraphGroups[i].labels[j][k]);
			}
			if (GraphGroups[i].labels[j].size() < 2)
				continue;
			for (int k = 0; k < SegNum; k++)
			{
				if (SegmentJointIndex[k] == 1)
					continue;
				int add = 0;
				for (int m = 0; m < GraphGroups[i].labels[j].size(); m++)
					if (SegAdjacencyMatrix(GraphGroups[i].labels[j][m], k) == 1)
					{
						add++;
					}
				if (add > 1)
					GraphGroups[i].joints[j].push_back(k);
			}
		}
		for (int j = 0; j < GraphGroups[i].labels.size(); j++)
		{
			QVector<int> allindex;
			for (int k = 0; k < GraphGroups[i].labels[j].size(); k++)
				allindex.push_back(GraphGroups[i].labels[j][k]);
			for (int k = 0; k < GraphGroups[i].joints[j].size(); k++)
				allindex.push_back(GraphGroups[i].joints[j][k]);
			GraphGroups[i].meshes[j] = mergedSeg(allindex, SorT);
		}
	}



	if (SorT == 0)
		SourceGraphGroups = GraphGroups;
	else
		TargetGraphGroups = GraphGroups;
}

bool CorrFinder::IsSmoothConnected(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> align, QVector<int> &type, double threshold)
{
	type.resize(groupA.labels.size());
	for (int i = 0; i < groupA.labels.size(); i++)
	{
		int tmptype;
		if (!IsSmoothConnected(groupA.SegmentAxis[i], groupB.SegmentAxis[align[i]], groupA.SegmentAxisDirection[i], groupB.SegmentAxisDirection[align[i]], tmptype, threshold))
			return false;
		type[i] = tmptype;
	}
	return true;
}

void CorrFinder::MergeSegToParts(int SorT)
{
	SurfaceMeshModel * proessShape;
	int SegNum;
	QVector<QVector<int>> SegmentIndex;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentAxis = SourceShapeSegmentAxis;
		SegmentAxisDirection = SourceShapeSegmentAxisDirection;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentAxis = TargetShapeSegmentAxis;
		SegmentAxisDirection = TargetShapeSegmentAxisDirection;
	}

	QVector<SegmentGroup> SegGroups;
	for (int i = 0; i < SegNum; i++)
	{
		if (SegmentJointIndex[i] == -1)
			continue;
		SegmentGroup newgroup;
		newgroup.labels.push_back(i);
		newgroup.members = mergedSeg(newgroup.labels, SorT);
		newgroup.SegmentAxis = SegmentAxis[i];
		newgroup.SegmentAxisDirection = SegmentAxisDirection[i];
		newgroup.SorT = SorT;
		SegGroups.push_back(newgroup);
	}

	int candidatesNum = SegGroups.size();
	for (int i = 0; i < SegGroups.size(); i++)
	{
		if (i > candidatesNum)
			candidatesNum = SegGroups.size();
		for (int j = 0; j < candidatesNum; j++)
		{
			if (i == j)
				continue;

			if (!IsAdjacented(SegGroups[i], SegGroups[j]))
				continue;

			int type = -1;
			if (!IsSmoothConnected(SegGroups[i], SegGroups[j], type))
				continue;

			SegmentGroup newgroup = MergeGroups(SegGroups[i], SegGroups[j], type);
			if (!IsExistedGroups(SegGroups, newgroup))
				SegGroups.push_back(newgroup);
		}
	}

	for (int i = 0; i < SegGroups.size(); i++)
	{
		if (SegGroups[i].labels.size() < 2)
			continue;
		for (int j = 0; j < SegNum; j++)
		{
			if (SegmentJointIndex[j] == 1)
				continue;
			int add = 0;
			for (int k = 0; k < SegGroups[i].labels.size(); k++)
				if (SegAdjacencyMatrix(SegGroups[i].labels[k], j) == 1)
				{
					add ++;
				}
			if (add > 1)
				SegGroups[i].labels.push_back(j);
		}
	}

	if (SorT == 0)
		SourceSegGroups = SegGroups;
	else
		TargetSegGroups = SegGroups;
}

bool CorrFinder::IsExistedGroups(QVector<SegmentGroup> groups, SegmentGroup test)
{
	for (int i = 0; i < groups.size(); i++)
		if (QVectorisEqual(groups[i].labels, test.labels))
			return true;
	return false;
}

bool CorrFinder::IsExistedGroups(QVector<SegmentGroupFromGraph> groups, SegmentGroupFromGraph test)
{
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].labels.size() != test.labels.size())
			continue;
		for (int j = 0; j < groups[i].labels.size(); j++)
		{
			for (int k = 0; k < groups[i].labels.size(); k++)
			{
				if (groups[i].labels[j].size() != test.labels[k].size())
					continue;
				if (QVectorisEqual(groups[i].labels[j], test.labels[k]))
					return true;
			}
		}
	}
	return false;
}

bool CorrFinder::IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT, QVector<int> &align)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	if (groupA.labels.size() != groupB.labels.size())
		return false;

	Eigen::MatrixXd CorrCandidates = Eigen::MatrixXd::Zero(groupA.labels.size(), groupA.labels.size());
	for (int i = 0; i < groupA.labels.size(); i++)
	{
		for (int j = 0; j < groupB.labels.size(); j++)
		{
			int err = 0;
			if (!IsAdjacented(groupA.labels[i], groupB.labels[j], SorT, err))
			{
				if (err == 1)
					return false;
				continue;
			}
			CorrCandidates(i, j) = 1;
		}
		if (CorrCandidates.row(i).sum() == 0)
			return false;
	}

	QVector<int> test;
	test.resize(groupA.labels.size());
	QVector<QVector<int>> wholeset;
	Permutation(groupA.labels.size(), test, wholeset, 0);

	int flag = 0;
	for (int k = 0; k < wholeset.size(); k++)
	{
		align = wholeset[k];
		flag = 0;
		for (int i = 0; i < groupA.labels.size(); i++)
			if (CorrCandidates(i, align[i]) == 1)
				flag++;
		if (flag == groupA.labels.size())
		{
			break;
		}
	}

	if (flag != groupA.labels.size())
		return false;

	return true;
}

bool CorrFinder::IsAdjacented(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, int SorT)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	if (groupA.labels.size() != groupB.labels.size())
		return false;

	Eigen::MatrixXd CorrCandidates = Eigen::MatrixXd::Zero(groupA.labels.size(), groupA.labels.size());
	for (int i = 0; i < groupA.labels.size(); i++)
	{
		for (int j = 0; j < groupB.labels.size(); j++)
		{
			int err = 0;
			if (!IsAdjacented(groupA.labels[i], groupB.labels[j], SorT, err))
			{
				if (err == 1)
					return false;
				continue;
			}
			CorrCandidates(i, j) = 1;
		}
		if (CorrCandidates.row(i).sum() == 0)
			return false;
	}
	return true;
}

bool CorrFinder::IsAdjacented(QVector<int> indexA, QVector<int> indexB, int SorT, int &err)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	bool flag = false;
	for (int i = 0; i < indexA.size(); i++)
	{
		for (int j = 0; j < indexB.size(); j++)
		{
			if (indexA[i] == indexB[j])
			{
				err = 1;
				return false;
			}
			if (SegAdjacencyMatrix(indexA[i], indexB[j]) == 1)
				flag = true;
		}
	}
	return flag;
}

bool CorrFinder::IsAdjacented(SegmentGroup groupA, SegmentGroup groupB)
{
	Eigen::MatrixXd SegAdjacencyMatrix;

	if (groupA.SorT == 0)
	{
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
	}
	else
	{
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
	}

	for (int i = 0; i < groupA.labels.size(); i++)
	{
		for (int j = 0; j < groupB.labels.size(); j++)
		{
			if (groupA.labels[i] == groupB.labels[j])
				return true;
			if (SegAdjacencyMatrix(groupA.labels[i], groupB.labels[j]) == 1)
				return true;
		}
	}
	return false;
}

SegmentGroupFromGraph CorrFinder::MergeGroups(SegmentGroupFromGraph groupA, SegmentGroupFromGraph groupB, QVector<int> type, QVector<int> align)
{
	SegmentGroupFromGraph newgroup;

	newgroup.labels.resize(groupA.labels.size());
	newgroup.SegmentAxis.resize(groupA.labels.size());
	newgroup.SegmentAxisDirection.resize(groupA.labels.size());

	for (int i = 0; i < groupA.labels.size(); i++)
	{
		newgroup.labels[i] = groupA.labels[i];
		for (int j = 0; j < groupB.labels[align[i]].size(); j++)
		{
			newgroup.labels[i].push_back(groupB.labels[align[i]][j]);
		}
		switch (type[i])
		{
		case 0:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_front(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_front(v);
			break;
		case 1:
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			break;
		case 2:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxis[align[i]])
				newgroup.SegmentAxis[i].push_back(v);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for each (Eigen::Vector3d v in groupB.SegmentAxisDirection[align[i]])
				newgroup.SegmentAxisDirection[i].push_back(v);
			break;
		case 3:
			for each (Eigen::Vector3d v in groupA.SegmentAxis[i])
				newgroup.SegmentAxis[i].push_back(v);
			for (int k = groupB.SegmentAxis[align[i]].size() - 1; k > -1; k--)
				newgroup.SegmentAxis[i].push_back(groupB.SegmentAxis[align[i]][k]);
			for each (Eigen::Vector3d v in groupA.SegmentAxisDirection[i])
				newgroup.SegmentAxisDirection[i].push_back(v);
			for (int k = groupB.SegmentAxisDirection[align[i]].size() - 1; k > -1; k--)
				newgroup.SegmentAxisDirection[i].push_back(groupB.SegmentAxisDirection[align[i]][k]);
			break;
		}
	}

	return newgroup;
}

SegmentGroup CorrFinder::MergeGroups(SegmentGroup groupA, SegmentGroup groupB, int type)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> SegFaceNum;

	if (groupA.SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegFaceNum = SourceSegFaceNum;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegFaceNum = TargetSegFaceNum;
	}

	SegmentGroup newgroup;
	newgroup.SorT = groupA.SorT;

	newgroup.labels.clear();
	newgroup.SegmentAxis.clear();
	newgroup.SegmentAxisDirection.clear();

	for (int i = 0; i < groupA.labels.size(); i++)
		if (!newgroup.labels.contains(groupA.labels[i]))
			newgroup.labels.push_back(groupA.labels[i]);

	for (int i = 0; i < groupB.labels.size(); i++)
		if (!newgroup.labels.contains(groupB.labels[i]))
			newgroup.labels.push_back(groupB.labels[i]);

	Surface_mesh::Vertex_property<Vector3> points = proessShape->vertex_property<Vector3>("v:point");

	SurfaceMesh::SurfaceMeshModel * piece = new SurfaceMesh::SurfaceMeshModel;
	int totalFaceNum = 0;
	for each (int n in newgroup.labels)
		totalFaceNum += SegFaceNum[n];

	piece->reserve(uint(totalFaceNum * 3), uint(totalFaceNum * 6), uint(totalFaceNum));
	for (auto v : proessShape->vertices())
	{
		piece->add_vertex(points[v]);
	}

	int findex = 0;
	for (auto f : proessShape->faces()){
		std::vector<SurfaceMesh::Vertex> face;
		for (auto v : proessShape->vertices(f)) face.push_back(v);
		for (int i = 0; i < newgroup.labels.size(); i++)
			if (SegmentIndex[newgroup.labels[i]][findex] == 1)
			{
				piece->add_face(face);
				break;
			}
		findex++;
	}

	for (auto v : piece->vertices()) if (piece->is_isolated(v)) piece->remove_vertex(v);
	piece->garbage_collection();
	piece->updateBoundingBox();
	piece->update_face_normals();
	piece->update_vertex_normals();

	newgroup.members = piece;

	switch (type)
	{
	case 0:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_front(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_front(v);
		break;
	case 1:
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		break;
	case 2:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for each (Eigen::Vector3d v in groupB.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		break;
	case 3:
		for each (Eigen::Vector3d v in groupA.SegmentAxis)
			newgroup.SegmentAxis.push_back(v);
		for (int i = groupB.SegmentAxis.size() - 1; i > -1; i--)
			newgroup.SegmentAxis.push_back(groupB.SegmentAxis[i]);
		for each (Eigen::Vector3d v in groupA.SegmentAxisDirection)
			newgroup.SegmentAxisDirection.push_back(v);
		for (int i = groupB.SegmentAxisDirection.size() - 1; i > -1; i--)
			newgroup.SegmentAxisDirection.push_back(groupB.SegmentAxisDirection[i]);
		break;
	}

	return newgroup;
}

bool CorrFinder::IsSmoothConnected(QVector<Eigen::Vector3d> PosA, QVector<Eigen::Vector3d> PosB, QVector<Eigen::Vector3d> DirA, QVector<Eigen::Vector3d> DirB, int &type, double threshold)
{
	double distance[4];
	distance[0] = (PosA[0] - PosB[0]).norm();
	distance[1] = (PosA[0] - PosB[PosB.size() - 1]).norm();
	distance[2] = (PosA[PosA.size() - 1] - PosB[0]).norm();
	distance[3] = (PosA[PosA.size() - 1] - PosB[PosB.size() - 1]).norm();

	if (distance[0] <= distance[1] && distance[0] <= distance[2] && distance[0] <= distance[3])
	{
		type = 0;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[0] - PosB[i]).norm() < distance[0])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[i] - PosB[0]).norm() < distance[0])
				return false;
		}

		Eigen::Vector3d d1 = DirA[0].normalized();
		Eigen::Vector3d d2 = DirB[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[1] <= distance[0] && distance[1] <= distance[2] && distance[1] <= distance[3])
	{
		type = 1;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == PosB.size() - 1)
				continue;
			if ((PosA[0] - PosB[i]).norm() < distance[1])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[i] - PosB[PosB.size() - 1]).norm() < distance[1])
				return false;
		}

		Eigen::Vector3d d1 = DirA[0].normalized();
		Eigen::Vector3d d2 = DirB[PosB.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[2] <= distance[0] && distance[2] <= distance[3] && distance[2] <= distance[1])
	{
		type = 2;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == 0)
				continue;
			if ((PosA[PosA.size() - 1] - PosB[i]).norm() < distance[2])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == PosA.size() - 1)
				continue;
			if ((PosA[i] - PosB[0]).norm() < distance[2])
				return false;
		}

		Eigen::Vector3d d1 = DirA[PosA.size() - 1].normalized();
		Eigen::Vector3d d2 = DirB[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[3] <= distance[0] && distance[3] <= distance[2] && distance[3] <= distance[1])
	{
		type = 3;

		for (int i = 0; i < PosB.size(); i++)
		{
			if (i == PosB.size() - 1)
				continue;
			if ((PosA[PosA.size() - 1] - PosB[i]).norm() < distance[3])
				return false;
		}

		for (int i = 0; i < PosA.size(); i++)
		{
			if (i == PosA.size() - 1)
				continue;
			if ((PosA[i] - PosB[PosB.size() - 1]).norm() < distance[3])
				return false;
		}

		Eigen::Vector3d d1 = DirA[PosA.size() - 1].normalized();
		Eigen::Vector3d d2 = DirB[PosB.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}

	return false;
}

bool CorrFinder::IsSmoothConnected(SegmentGroup groupA, SegmentGroup groupB, int &type, double threshold)
{
	double distance[4];
	distance[0] = (groupA.SegmentAxis[0] - groupB.SegmentAxis[0]).norm();
	distance[1] = (groupA.SegmentAxis[0] - groupB.SegmentAxis[groupB.SegmentAxis.size()-1]).norm();
	distance[2] = (groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[0]).norm();
	distance[3] = (groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm();

	if (distance[0] <= distance[1] && distance[0] <= distance[2] && distance[0] <= distance[3])
	{
		type = 0;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[0] - groupB.SegmentAxis[i]).norm() < distance[0])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[0]).norm() < distance[0])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[0].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}	
	if (distance[1] <= distance[0] && distance[1] <= distance[2] && distance[1] <= distance[3])
	{
		type = 1;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == groupB.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[0] - groupB.SegmentAxis[i]).norm() < distance[1])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm() < distance[1])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[0].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[groupB.SegmentAxis.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[2] <= distance[0] && distance[2] <= distance[3] && distance[2] <= distance[1])
	{
		type = 2;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == 0)
				continue;
			if ((groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[i]).norm() < distance[2])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == groupA.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[0]).norm() < distance[2])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[groupA.SegmentAxis.size() - 1].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[0].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}
	if (distance[3] <= distance[0] && distance[3] <= distance[2] && distance[3] <= distance[1])
	{
		type = 3;

		for (int i = 0; i < groupB.SegmentAxis.size(); i++)
		{
			if (i == groupB.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[groupA.SegmentAxis.size() - 1] - groupB.SegmentAxis[i]).norm() < distance[3])
				return false;
		}

		for (int i = 0; i < groupA.SegmentAxis.size(); i++)
		{
			if (i == groupA.SegmentAxis.size() - 1)
				continue;
			if ((groupA.SegmentAxis[i] - groupB.SegmentAxis[groupB.SegmentAxis.size() - 1]).norm() < distance[3])
				return false;
		}

		Eigen::Vector3d d1 = groupA.SegmentAxisDirection[groupA.SegmentAxis.size() - 1].normalized();
		Eigen::Vector3d d2 = groupB.SegmentAxisDirection[groupB.SegmentAxis.size() - 1].normalized();
		double tmp = abs(d1.dot(d2));
		if (abs(d1.dot(d2)) > threshold)
			return true;
	}

	return false;
}

bool CorrFinder::IsFlatMerge(SegmentGroup groupA, SegmentGroup groupB)
{
	OBB_Volume obbA(groupA.members);
	OBB_Volume obbB(groupB.members);

	Eigen::Vector3d extantsA = obbA.extents();
	Eigen::Vector3d extantsB = obbB.extents();

	int minA = 0, minB = 0;
	for (int i = 1; i < 3; i++)
	{
		if (extantsA[i] < extantsA[minA])
			minA = i;
		if (extantsB[i] < extantsB[minB])
			minB = i;
	}

	if (abs(extantsA[minA] - extantsB[minB]) / std::min(extantsA[minA], extantsB[minB]) > 1)
		return false;

	std::vector<Vector3d> axisA = obbA.axis();
	std::vector<Vector3d> axisB = obbB.axis();

	axisA[minA].normalize();
	axisB[minB].normalize();

	if (abs(axisA[minA].dot(axisB[minB])) < 0.8)
		return false;

	Eigen::Vector3d centerA = obbA.center();
	Eigen::Vector3d centerB = obbB.center();

	Eigen::Vector3d meanAxis;

	if (axisA[minA].dot(axisB[minB]) < 0)
		meanAxis = (axisA[minA] - axisB[minB]) / 2;
	else
		meanAxis = (axisA[minA] + axisB[minB]) / 2;

	centerA = meanAxis.dot(centerA) * meanAxis;
	centerB = meanAxis.dot(centerB) * meanAxis;

	if ((centerA - centerB).norm() > std::max(extantsA[minA], extantsB[minB]) && (centerA + centerB).norm() > std::max(extantsA[minA], extantsB[minB]))
		return false;

	return true;
}

void CorrFinder::MergeStraightConnectedCylinders(int SorT)
{
	QVector<SurfaceMeshModel *> ShapeSegment;
	int SegNum;
	Eigen::MatrixXd SegAdjacencyMatrix;
	QVector<int> SegmentJointIndex;
	QVector<QVector<Eigen::Vector3d>> SegmentAxis;
	QVector<QVector<Eigen::Vector3d>> SegmentAxisDirection;

	if (SorT == 0)
	{
		ShapeSegment = SourceShapeSegment;
		SegNum = SourceShapeSegmentNum;
		SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
		SegmentJointIndex = SourceShapeSegmentJointIndex;
		SegmentAxis = SourceShapeSegmentAxis;
		SegmentAxisDirection = SourceShapeSegmentAxisDirection;
	}
	else
	{
		ShapeSegment = TargetShapeSegment;
		SegNum = TargetShapeSegmentNum;
		SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
		SegmentJointIndex = TargetShapeSegmentJointIndex;
		SegmentAxis = TargetShapeSegmentAxis;
		SegmentAxisDirection = TargetShapeSegmentAxisDirection;
	}

	for (int i = 0; i < SegNum; i++)
	{
		for (int j = i + 1; j < SegNum; j++)
		{
			if (SegAdjacencyMatrix(i, j) == 1 && SegmentJointIndex[i] == 1 && SegmentJointIndex[j] == 1)
			{
				bool flag = false;
				for (int k = 0; k < SegNum; k++)
					if (SegmentJointIndex[k] == -1 && SegAdjacencyMatrix(k, j) == 1 && SegAdjacencyMatrix(k, i) == 1)
					{
						flag = true;
					}
				if (flag) continue;
				
				int type;
				if (IsSmoothConnected(SegmentAxis[i], SegmentAxis[j], SegmentAxisDirection[i], SegmentAxisDirection[j], type, 0.8))
				{
					QVector<Eigen::Vector3d> newaxis;
					QVector<Eigen::Vector3d> newaxisdirection;
					switch (type)
					{
					case 0:
						for each (Eigen::Vector3d v in SegmentAxis[i])
							newaxis.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxis[j])
							newaxis.push_front(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[i])
							newaxisdirection.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[j])
							newaxisdirection.push_front(v);
						break;
					case 1:
						for each (Eigen::Vector3d v in SegmentAxis[j])
							newaxis.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxis[i])
							newaxis.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[j])
							newaxisdirection.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[i])
							newaxisdirection.push_back(v);
						break;
					case 2:
						for each (Eigen::Vector3d v in SegmentAxis[i])
							newaxis.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxis[j])
							newaxis.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[i])
							newaxisdirection.push_back(v);
						for each (Eigen::Vector3d v in SegmentAxisDirection[j])
							newaxisdirection.push_back(v);
						break;
					case 3:
						for each (Eigen::Vector3d v in SegmentAxis[i])
							newaxis.push_back(v);
						for (int k = SegmentAxis[j].size() - 1; k > -1; k--)
							newaxis.push_back(SegmentAxis[j][k]);
						for each (Eigen::Vector3d v in SegmentAxisDirection[i])
							newaxisdirection.push_back(v);
						for (int k = SegmentAxisDirection[j].size() - 1; k > -1; k--)
							newaxisdirection.push_back(SegmentAxisDirection[j][k]);
						break;
					}

					MergeTwoSegs(i, j, SorT);
					j = i + 1;
					SegNum--;
					SegmentAxis[i] = newaxis;
					SegmentAxisDirection[i] = newaxisdirection;
					if (SorT == 0)
					{
						SegAdjacencyMatrix = SourceSegAdjacencyMatrix;
						SegmentJointIndex = SourceShapeSegmentJointIndex;
						SourceShapeSegmentAxis[i] = newaxis;
						SourceShapeSegmentAxisDirection[i] = newaxisdirection;
					}
					else
					{
						SegAdjacencyMatrix = TargetSegAdjacencyMatrix;
						SegmentJointIndex = TargetShapeSegmentJointIndex;
						TargetShapeSegmentAxis[i] = newaxis;
						TargetShapeSegmentAxisDirection[i] = newaxisdirection;
					}
				}
			}
		}
	}
}

void CorrFinder::GeneratePartSet()
{
	FindSegAdjacencyMatrix();
	FlatSegMerge(2.5, 0, SourceShapeSegmentFlatIndex);
	FlatSegMerge(2.5, 1, TargetShapeSegmentFlatIndex);

	GetSegFaceNum();
	GenerateSegMeshes(0);
	GenerateSegMeshes(1);
	FindSegAdjacencyMatrix();
	GenerateGroupsFromGraph();
	MergeGraphSegToParts(0);
	MergeGraphSegToParts(1);
	samplePartialGraphs();
//	buildStructureGraph();
}

void CorrFinder::samplePartialGraphs(int step, int totalNum)
{
	srand((unsigned)time(NULL));
	AdjacencySourceGraphGroups = Eigen::MatrixXd::Identity(SourceGraphGroups.size(), SourceGraphGroups.size());
	Eigen::MatrixXd BanSourceGraphGroups = Eigen::MatrixXd::Identity(SourceGraphGroups.size(), SourceGraphGroups.size());
	for (int i = 0; i < SourceGraphGroups.size(); i++)
	{
		for (int j = i + 1; j < SourceGraphGroups.size(); j++)
		{
			int err = 0;
			if (IsAdjacented(SourceGraphGroups[i].allseg, SourceGraphGroups[j].allseg, 0, err))
			{
				AdjacencySourceGraphGroups(i, j) = 1;
				AdjacencySourceGraphGroups(j, i) = 1;
			}
			if (err == 1)
			{
				BanSourceGraphGroups(i, j) = 1;
				BanSourceGraphGroups(j, i) = 1;
			}
		}
	}
	SourceStructureGraphs.clear();
	QVector<QVector<int>> visited;
	while (SourceStructureGraphs.size() < totalNum)
	{
		InitialStructureGraph newGraph;

		int added = 1;
		QVector<int> newVisited;
		int seedIndex = rand() % SourceGraphGroups.size();
		newVisited.push_back(seedIndex);
		insertNodeInGraph(newGraph, SourceGraphGroups[seedIndex], 0);
		int currentNode = seedIndex;
		while (added < step)
		{
			int tmpcurrent;
			int l = AdjacencySourceGraphGroups.row(currentNode).sum() - 1;
			int tmp = rand() % l + 1;
			for (int i = 0; i < SourceGraphGroups.size(); i++)
			{
				if (AdjacencySourceGraphGroups(currentNode, i) == 1)
				{
					tmp--;
				}
				if (tmp == 0)
				{
					tmpcurrent = i;
					break;
				}
			}
			if (insertNodeInGraph(newGraph, SourceGraphGroups[tmpcurrent], 0))
			{
				added++;
				for (int i = 0; i < newVisited.size(); i++)
				{
					if (tmpcurrent < newVisited[i])
					{
						newVisited.insert(newVisited.begin() + i, tmpcurrent);
						break;
					}
					if (i == newVisited.size() - 1)
					{
						newVisited.push_back(tmpcurrent);
						break;
					}
						
				}
				currentNode = tmpcurrent;
			}
		}

		if (!visited.contains(newVisited))
		{
			newGraph.edgeCoordA.resize(newGraph.edges.size());
			newGraph.edgeCoordB.resize(newGraph.edges.size());
			newGraph.sheets.resize(newGraph.curves.size());
			/////////////////////////////////////////////////
			for (int i = 0; i < newGraph.meshes.size(); i++)
			{
				if (newGraph.flatIndex[i] != 1)
					continue;
				OBB_Volume obb(newGraph.meshes[i]);
				Eigen::Vector3d du, dv;
				double lu, lv;
				obb.First2Axis(du, dv, lu, lv);
				newGraph.sheets[i] = NURBS::NURBSRectangled::createSheet(lu, lv, obb.center(), du, dv).mCtrlPoint;
			}
			SourceStructureGraphs.push_back(newGraph);
			visited.push_back(newVisited);
		}
			
	}

	///////////////////
	
}

bool CorrFinder::insertNodeInGraph(InitialStructureGraph &graph, SegmentGroupFromGraph node, int SorT)
{
	for (int i = 0; i < node.labels.size(); i++)
	{
		for (int j = 0; j < node.labels[i].size(); j++)
		{
			for (int k = 0; k < graph.indexes.size(); k++)
			{
				for (int n = 0; n < graph.indexes[k].size(); n++)
					if (node.labels[i][j] == graph.indexes[k][n])
						return false;
			}
		}
	}
	QVector<int> groupsIndex;
	for (int i = 0; i < node.labels.size(); i++)
	{
		for (int j = 0; j < graph.meshes.size(); j++)
		{
			if (graph.edges.contains(QPair<int, int>(graph.meshes.size(), j)) || graph.edges.contains(QPair<int, int>(j, graph.meshes.size())))
				continue;
			int err;
			if (IsAdjacented(node.labels[i], graph.indexes[j], SorT, err))
				graph.edges.push_back(QPair<int, int>(graph.meshes.size(), j));
		}
		graph.meshes.push_back(node.meshes[i]);
		graph.curves.push_back(node.SegmentAxis[i]);
		graph.indexes.push_back(node.labels[i]);
		if (node.flat == 1)
			graph.flatIndex.push_back(1);
		else
			graph.flatIndex.push_back(0);
		int index = graph.curves.size() - 1;
		while (graph.curves[index].size() < 4)
		{
			Eigen::Vector3d middle = graph.curves[index][0] + graph.curves[index][1];
			middle = middle / 2;
			graph.curves[index].insert(graph.curves[index].begin() + 1, middle);
		}
		groupsIndex.push_back(graph.meshes.size() - 1);
	}
	graph.groups.push_back(groupsIndex);
	return true;
}

void CorrFinder::buildStructureGraph()
{
	for (int i = 0; i < SourceShapeSegment.size(); i++)
	{
		if (SourceShapeSegmentJointIndex[i] == -1)
			continue;
		SourceStructureGraph.meshes.push_back(SourceShapeSegment[i]);
		SourceStructureGraph.curves.push_back(SourceShapeSegmentAxis[i]);
		if (SourceShapeSegmentFlatIndex[i] == 1)
			SourceStructureGraph.flatIndex.push_back(1);
		else
			SourceStructureGraph.flatIndex.push_back(0);
		for (int j = i + 1; j < SourceShapeSegment.size(); j++)
		{
			if (SourceShapeSegmentJointIndex[j] == -1)
				continue;
			if (SourceSegAdjacencyMatrix(i, j) == 1)
			{
				SourceStructureGraph.edges.push_back(QPair<int, int>(i, j));
			}
		}
		int index = SourceStructureGraph.curves.size() - 1;
		while (SourceStructureGraph.curves[index].size() < 4)
		{
			Eigen::Vector3d middle = SourceStructureGraph.curves[index][0] + SourceStructureGraph.curves[index][1];
			middle = middle / 2;
			SourceStructureGraph.curves[index].insert(SourceStructureGraph.curves[index].begin() + 1, middle);
		}
	}
	SourceStructureGraph.edgeCoordA.resize(SourceStructureGraph.edges.size());
	SourceStructureGraph.edgeCoordB.resize(SourceStructureGraph.edges.size());
	SourceStructureGraph.sheets.resize(SourceStructureGraph.curves.size());
	/////////////////////////////////////////////////
	for (int i = 0; i < SourceShapeSegment.size(); i++)
	{
		if (SourceShapeSegmentJointIndex[i] == -1 || SourceShapeSegmentFlatIndex[i] != 1)
			continue;
		OBB_Volume obb(SourceStructureGraph.meshes[i]);
		Eigen::Vector3d du, dv;
		double lu, lv;
		obb.First2Axis(du, dv, lu, lv);
		SourceStructureGraph.sheets[i] = NURBS::NURBSRectangled::createSheet(lu, lv, obb.center(), du, dv).mCtrlPoint;
	}
	/////////////////////////////////////////////////

	for (int i = 0; i < SourceUnoverlapGraphGroups.size(); i++)
	{
		if (SourceUnoverlapGraphGroups[i].labels.size() == 1)
			continue;
		QVector<int> group;
		for (int j = 0; j < SourceUnoverlapGraphGroups[i].labels.size(); j++)
			group.push_back(SourceUnoverlapGraphGroups[i].labels[j][0]);
		SourceStructureGraph.groups.push_back(group);
	}

	for (int i = 0; i < TargetShapeSegment.size(); i++)
	{
		if (TargetShapeSegmentJointIndex[i] == -1)
			continue;
		TargetStructureGraph.meshes.push_back(TargetShapeSegment[i]);
		TargetStructureGraph.curves.push_back(TargetShapeSegmentAxis[i]);
		if (TargetShapeSegmentFlatIndex[i] == 1)
			TargetStructureGraph.flatIndex.push_back(1);
		else
			TargetStructureGraph.flatIndex.push_back(0);
		for (int j = i + 1; j < TargetShapeSegment.size(); j++)
		{
			if (TargetShapeSegmentJointIndex[j] == -1)
				continue;
			if (TargetSegAdjacencyMatrix(i, j) == 1)
			{
				TargetStructureGraph.edges.push_back(QPair<int, int>(i, j));
			}
		}
		int index = TargetStructureGraph.curves.size() - 1;
		while (TargetStructureGraph.curves[index].size() < 4)
		{
			Eigen::Vector3d middle = TargetStructureGraph.curves[index][0] + TargetStructureGraph.curves[index][1];
			middle = middle / 2;
			TargetStructureGraph.curves[index].insert(TargetStructureGraph.curves[index].begin() + 1, middle);
		}
	}
	TargetStructureGraph.edgeCoordA.resize(TargetStructureGraph.edges.size());
	TargetStructureGraph.edgeCoordB.resize(TargetStructureGraph.edges.size());
	TargetStructureGraph.sheets.resize(TargetStructureGraph.curves.size());
	/////////////////////////////////////////////////
	for (int i = 0; i < TargetShapeSegment.size(); i++)
	{
		if (TargetShapeSegmentJointIndex[i] == -1 || TargetShapeSegmentFlatIndex[i] != 1)
			continue;
		OBB_Volume obb(TargetStructureGraph.meshes[i]);
		Eigen::Vector3d du, dv;
		double lu, lv;
		obb.First2Axis(du, dv, lu, lv);
		TargetStructureGraph.sheets[i] = NURBS::NURBSRectangled::createSheet(lu, lv, obb.center(), du, dv).mCtrlPoint;
	}
	/////////////////////////////////////////////////
	for (int i = 0; i < TargetUnoverlapGraphGroups.size(); i++)
	{
		if (TargetUnoverlapGraphGroups[i].labels.size() == 1)
			continue;
		QVector<int> group;
		for (int j = 0; j < TargetUnoverlapGraphGroups[i].labels.size(); j++)
			group.push_back(TargetUnoverlapGraphGroups[i].labels[j][0]);
		TargetStructureGraph.groups.push_back(group);
	}
}

void CorrFinder::generateSheetPara()
{
/*	//////////////////////////////////////////////////////
	for (int i = 0; i < SourceUnoverlapGraphGroups.size(); i++)
	{
		if (SourceUnoverlapGraphGroups[i].labels.size() != 1)
			continue;
		if (SourceShapeSegmentFlatIndex[SourceUnoverlapGraphGroups[i].labels[0][0]] != 1)
			continue;
		FilterPlugin * matPlugin = pluginManager()->getFilter("Skeleton | Voronoi based MAT");
		RichParameterSet * mat_params = new RichParameterSet;
		matPlugin->initParameters(mat_params);
		document()->setSelectedModel(SourceShapeSegment[SourceUnoverlapGraphGroups[i].labels[0][0]]);
		matPlugin->applyFilter(mat_params);
		for (int j = 0; j < 3; j++)
		{
			FilterPlugin * mcfPlugin = pluginManager()->getFilter("Skeleton | MCF Skeletonization");

			RichParameterSet * mcf_params = new RichParameterSet;
			mcfPlugin->initParameters(mcf_params);
			mcf_params->setValue("omega_P_0", 0.0f);
			mcfPlugin->applyFilter(mcf_params);
		}
		SourceUnoverlapGraphGroups[i].SheetAxis = surfaceFit(SourceShapeSegment[SourceUnoverlapGraphGroups[i].labels[0][0]]);
		SourceUnoverlapGraphGroups[i].flat = 1;
	}

	for (int i = 0; i < TargetUnoverlapGraphGroups.size(); i++)
	{
		if (TargetUnoverlapGraphGroups[i].labels.size() != 1)
			continue;
		if (TargetShapeSegmentFlatIndex[TargetUnoverlapGraphGroups[i].labels[0][0]] != 1)
			continue;
		FilterPlugin * matPlugin = pluginManager()->getFilter("Voronoi based MAT");
		RichParameterSet * mat_params = new RichParameterSet;
		matPlugin->initParameters(mat_params);
		document()->setSelectedModel(TargetShapeSegment[TargetUnoverlapGraphGroups[i].labels[0][0]]);
		matPlugin->applyFilter(mat_params);
		for (int j = 0; j < 3; j++)
		{
			FilterPlugin * mcfPlugin = pluginManager()->getFilter("MCF Skeletonization");

			RichParameterSet * mcf_params = new RichParameterSet;
			mcfPlugin->initParameters(mcf_params);
			mcf_params->setValue("omega_P_0", 0.0f);
			mcfPlugin->applyFilter(mcf_params);
		}
		TargetUnoverlapGraphGroups[i].SheetAxis = surfaceFit(TargetShapeSegment[TargetUnoverlapGraphGroups[i].labels[0][0]]);
		TargetUnoverlapGraphGroups[i].flat = 1;
	}*/
}

void CorrFinder::GenerateGroupsFromGraph()
{
	SegGraph GraphS;
	int rnum = 0;
	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		if (SourceShapeSegmentJointIndex[i] == -1)
			continue;
		SegGraphNode tmp;
		tmp.labels.push_back(i);
		tmp.lowest = SourceShapeSegment[i]->bbox().min()[2];
		tmp.feature = SourceShapeSegment[i]->bbox().max() - SourceShapeSegment[i]->bbox().min();
		GraphS.AddNode(tmp);
		rnum++;
	}
	SourceRealSegAdjacencyMatrix = SourceSegAdjacencyMatrix.block(0, 0, rnum, rnum);
	GraphS.AddAdjacencyMatrix(SourceRealSegAdjacencyMatrix);
	GraphS.BuildInitialGraph();
	GraphS.GenerateGroups();

	for (int i = 0; i < GraphS.groups.size(); i++)
	{
		SegmentGroupFromGraph tmp;
		for (int j = 0; j < GraphS.groups[i].size(); j++)
		{
			QVector<int> tmplabel;
			tmplabel.push_back(GraphS.groups[i][j]);
			tmp.labels.push_back(tmplabel);
			tmp.SegmentAxis.push_back(SourceShapeSegmentAxis[GraphS.groups[i][j]]);
			tmp.SegmentAxisDirection.push_back(SourceShapeSegmentAxisDirection[GraphS.groups[i][j]]);
			for (int k = 0; k < tmplabel.size(); k++)
			{
				if (SourceShapeSegmentFlatIndex[tmplabel[k]] == 1)
					tmp.flat = 1;
			}
		}
		SourceGraphGroups.push_back(tmp);
	}
	SourceUnoverlapGraphGroups = SourceGraphGroups;

	SegGraph GraphT;
	rnum = 0;
	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		if (TargetShapeSegmentJointIndex[i] == -1)
			continue;
		SegGraphNode tmp;
		tmp.labels.push_back(i);
		tmp.lowest = TargetShapeSegment[i]->bbox().min()[2];
		tmp.feature = TargetShapeSegment[i]->bbox().max() - TargetShapeSegment[i]->bbox().min();
		GraphT.AddNode(tmp);
		rnum++;
	}
	TargetRealSegAdjacencyMatrix = TargetSegAdjacencyMatrix.block(0, 0, rnum, rnum);
	GraphT.AddAdjacencyMatrix(TargetRealSegAdjacencyMatrix);
	GraphT.BuildInitialGraph();
	GraphT.GenerateGroups();

	for (int i = 0; i < GraphT.groups.size(); i++)
	{
		SegmentGroupFromGraph tmp;
		for (int j = 0; j < GraphT.groups[i].size(); j++)
		{
			QVector<int> tmplabel;
			tmplabel.push_back(GraphT.groups[i][j]);
			tmp.labels.push_back(tmplabel);
			tmp.SegmentAxis.push_back(TargetShapeSegmentAxis[GraphT.groups[i][j]]);
			tmp.SegmentAxisDirection.push_back(TargetShapeSegmentAxisDirection[GraphT.groups[i][j]]);
			for (int k = 0; k < tmplabel.size(); k++)
			{
				if (TargetShapeSegmentFlatIndex[tmplabel[k]] == 1)
					tmp.flat = 1;
			}
		}
		TargetGraphGroups.push_back(tmp);
	}
	TargetUnoverlapGraphGroups = TargetGraphGroups;
}

void CorrFinder::DrawSpecificPart(int index, int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<SegmentGroupFromGraph> SegGroups;

	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegmentIndex = SourceShapeSegmentIndex;
		SegGroups = SourceGraphGroups;
	}
	else
	{
		proessShape = TargetShape;
		SegmentIndex = TargetShapeSegmentIndex;
		SegGroups = TargetGraphGroups;
	}
	Surface_mesh::Face_property<QColor> fcolors = proessShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = proessShape->faces_end();

	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit){
		fcolors[fit] = QColor(255, 255, 255);
		for (int i = 0; i < SegGroups[index].labels.size(); i++)
		{
			bool flag = false;
			for (int j = 0; j < SegGroups[index].labels[i].size(); j++)
			{
				if (SegmentIndex[SegGroups[index].labels[i][j]][findex] == 1)
				{
					fcolors[fit] = ColorDifferent[i % 6];
					flag = true;
					break;
				}
			}

			if (flag)
			{
				i = SegGroups[index].labels.size();
				break;
			}

			for (int j = 0; j < SegGroups[index].joints[i].size(); j++)
			{
				if (SegmentIndex[SegGroups[index].joints[i][j]][findex] == 1)
				{
					fcolors[fit] = ColorDifferent[i % 6];
					flag = true;
					break;
				}
			}

			if (flag)
			{
				i = SegGroups[index].labels.size();
				break;
			}
		}
		findex++;
	}
}

/*void CorrFinder::GenerateInitialGroups(double t)
{
	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		Eigen::AlignedBox3d mbboxi = SourceShapeSegment[i]->bbox();

		SymmetryGroup tmpGroup;
		tmpGroup.members.push_back(SourceShapeSegment[i]);
		tmpGroup.labels.push_back(i);

		for (int j = i + 1; j < SourceShapeSegmentNum; j++)
		{
			double sum = 0;
			Eigen::AlignedBox3d mbboxj = SourceShapeSegment[j]->bbox();
			sum += abs(mbboxi.sizes()[0] - mbboxj.sizes()[0]);
			sum += abs(mbboxi.sizes()[1] - mbboxj.sizes()[1]);
			sum += abs(mbboxi.sizes()[2] - mbboxj.sizes()[2]);

			double threshold = std::min(mbboxi.sizes()[0] + mbboxi.sizes()[1] + mbboxi.sizes()[2], mbboxj.sizes()[0] + mbboxj.sizes()[1] + mbboxj.sizes()[2]);
			if (sum / threshold < t)
			{
				tmpGroup.members.push_back(SourceShapeSegment[j]);
				tmpGroup.labels.push_back(j);
			}
		}

		bool isAdded = false;
		for (int j = 0; j < SourceSegGroups.size(); j++)
		{
			int matched = 0;
			for (int k = 0; k < tmpGroup.labels.size(); k++)
			{
				if (std::find(SourceSegGroups[j].labels.begin(), SourceSegGroups[j].labels.end(), tmpGroup.labels[k]) != SourceSegGroups[j].labels.end())
					matched++;
			}
			if (matched == tmpGroup.labels.size())
			{
				isAdded = true;
				break;
			}
		}
		if (!isAdded)
			SourceSegGroups.push_back(tmpGroup);
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		Eigen::AlignedBox3d mbboxi = TargetShapeSegment[i]->bbox();

		SymmetryGroup tmpGroup;
		tmpGroup.members.push_back(TargetShapeSegment[i]);
		tmpGroup.labels.push_back(i);

		for (int j = i + 1; j < TargetShapeSegmentNum; j++)
		{
			double sum = 0;
			Eigen::AlignedBox3d mbboxj = TargetShapeSegment[j]->bbox();
			sum += abs(mbboxi.sizes()[0] - mbboxj.sizes()[0]);
			sum += abs(mbboxi.sizes()[1] - mbboxj.sizes()[1]);
			sum += abs(mbboxi.sizes()[2] - mbboxj.sizes()[2]);

			double threshold = std::min(mbboxi.sizes()[0] + mbboxi.sizes()[1] + mbboxi.sizes()[2], mbboxj.sizes()[0] + mbboxj.sizes()[1] + mbboxj.sizes()[2]);
			if (sum / threshold < t)
			{
				tmpGroup.members.push_back(TargetShapeSegment[j]);
				tmpGroup.labels.push_back(j);
			}
		}

		bool isAdded = false;
		for (int j = 0; j < TargetSegGroups.size(); j++)
		{
			int matched = 0;
			for (int k = 0; k < tmpGroup.labels.size(); k++)
			{
				if (std::find(TargetSegGroups[j].labels.begin(), TargetSegGroups[j].labels.end(), tmpGroup.labels[k]) != TargetSegGroups[j].labels.end())
					matched++;
			}
			if (matched == tmpGroup.labels.size())
			{
				isAdded = true;
				break;
			}
		}
		if (!isAdded)
			TargetSegGroups.push_back(tmpGroup);
	}

}*/

void CorrFinder::ApplyPartColor(int sindex)
{
	Surface_mesh::Face_property<QColor> fscolors = SourceShape->face_property<QColor>("f:partcolor");
	Surface_mesh::Face_property<QColor> ftcolors = TargetShape->face_property<QColor>("f:partcolor");

	Surface_mesh::Face_iterator fit, fend = SourceShape->faces_end();

	double eachInternal = 1.0f / SourceShapePartNum;
	int index = 0;
	for (fit = SourceShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < SourceShapePartNum; i++)
				if (SourceShapePartIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (SourceShapePartIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		fscolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}

	fend = TargetShape->faces_end();

	eachInternal = 1.0f / TargetShapePartNum;
	index = 0;
	for (fit = TargetShape->faces_begin(); fit != fend; ++fit){
		int selected;
		if (sindex == -1)
		{
			for (int i = 0; i < TargetShapePartNum; i++)
				if (TargetShapePartIndex[i][index] == 1)
				{
					selected = i;
					break;
				}
		}
		else if (TargetShapePartIndex[sindex][index] == 1)
			selected = sindex;
		else
		{
			index++;
			continue;
		}
		ftcolors[fit] = getColorFromMap(eachInternal*selected, ColorMap);
		index++;
	}
}

bool CorrFinder::LoadParialPartFile()
{
	QFile sourceparialpart(sourceIndex_path);
	QFile targetparialpart(targetIndex_path);
	if (!sourceparialpart.open(QIODevice::ReadOnly) || !targetparialpart.open(QIODevice::ReadOnly))
		return false;
	QTextStream in_source(&sourceparialpart);
	QTextStream in_target(&targetparialpart);

	QString str;
	QStringList strtmp;

	str = in_source.readLine();
	strtmp = str.split(" ");
	SourceShapeSegmentNum = strtmp[0].toInt();
	sourceVaildSegNum = strtmp[1].toInt();
	SourceShapeSegmentIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxis.resize(SourceShapeSegmentNum);
	SourceShapeSegmentAxisDirection.resize(SourceShapeSegmentNum);
	SourceShapeSegmentGC.resize(SourceShapeSegmentNum);

	str = in_target.readLine();
	strtmp = str.split(" ");
	TargetShapeSegmentNum = strtmp[0].toInt();
	targetVaildSegNum = strtmp[1].toInt();
	TargetShapeSegmentIndex.resize(TargetShapeSegmentNum);
	TargetShapeSegmentAxis.resize(TargetShapeSegmentNum);
	TargetShapeSegmentAxisDirection.resize(TargetShapeSegmentNum);
	TargetShapeSegmentGC.resize(TargetShapeSegmentNum);

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		str = in_source.readLine();
		str = in_source.readLine();
		SourceShapeSegmentGC[i] = str.toDouble();
		for (int j = 0; j < 6; j++)
			str = in_source.readLine();
		int axisNum = str.toInt();
		SourceShapeSegmentAxis[i].resize(axisNum);
		SourceShapeSegmentAxisDirection[i].resize(axisNum);

		for (int j = 0; j < axisNum; j++)
		{
			str = in_source.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d vertex(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			SourceShapeSegmentAxis[i][j] = vertex;

			str = in_source.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d direction(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			SourceShapeSegmentAxisDirection[i][j] = direction;
		}

		str = in_source.readLine();
		int faceNum = str.toInt();
		str = in_source.readLine();
		strtmp = str.split(" ");

		SourceShapeSegmentIndex[i].resize(SourceShape->faces_size());
		SourceShapeSegmentIndex[i].fill(0);
		for (int j = 0; j < faceNum; j++)
		{
			SourceShapeSegmentIndex[i][strtmp[j].toInt()] = 1;
		}
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		str = in_target.readLine();
		str = in_target.readLine();
		TargetShapeSegmentGC[i] = str.toDouble();
		for (int j = 0; j < 6; j++)
			str = in_target.readLine();
		int axisNum = str.toInt();
		TargetShapeSegmentAxis[i].resize(axisNum);
		TargetShapeSegmentAxisDirection[i].resize(axisNum);

		for (int j = 0; j < axisNum; j++)
		{
			str = in_target.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d vertex(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			TargetShapeSegmentAxis[i][j] = vertex;

			str = in_target.readLine();
			strtmp = str.split(" ");
			Eigen::Vector3d direction(strtmp[0].toDouble(), strtmp[1].toDouble(), strtmp[2].toDouble());
			TargetShapeSegmentAxisDirection[i][j] = direction;
		}

		str = in_target.readLine();
		int faceNum = str.toInt();
		str = in_target.readLine();
		strtmp = str.split(" ");

		TargetShapeSegmentIndex[i].resize(TargetShape->faces_size());
		TargetShapeSegmentIndex[i].fill(0);
		for (int j = 0; j < faceNum; j++)
		{
			TargetShapeSegmentIndex[i][strtmp[j].toInt()] = 1;
		}
	}

	str = in_source.readLine();
	str = in_source.readLine();
	str = in_target.readLine();
	str = in_target.readLine();

	while (!in_source.atEnd())
	{
		str = in_source.readLine();
		SourceRealSegIndex.push_back(str.toInt());
	}

	while (!in_target.atEnd())
	{
		str = in_target.readLine();
		TargetRealSegIndex.push_back(str.toInt());
	}

	for (int i = 0; i < SourceShapeSegmentNum; i++)
	{
		if (SourceRealSegIndex[i] == i)
			continue;
		for (int j = 0; j < SourceShapeSegmentIndex[i].size(); j++)
		{
			if (SourceShapeSegmentIndex[i][j])
				SourceShapeSegmentIndex[SourceRealSegIndex[i]][j] = 1;
		}
		SourceRealSegIndex.erase(SourceRealSegIndex.begin()+i);
		SourceShapeSegmentIndex.erase(SourceShapeSegmentIndex.begin()+i);
		SourceShapeSegmentAxis.erase(SourceShapeSegmentAxis.begin()+i);
		SourceShapeSegmentAxisDirection.erase(SourceShapeSegmentAxisDirection.begin() + i);
		i--;
		SourceShapeSegmentNum--;
	}

	for (int i = 0; i < TargetShapeSegmentNum; i++)
	{
		if (TargetRealSegIndex[i] == i)
			continue;
		for (int j = 0; j < TargetShapeSegmentIndex[i].size(); j++)
		{
			if (TargetShapeSegmentIndex[i][j])
				TargetShapeSegmentIndex[TargetRealSegIndex[i]][j] = 1;
		}
		TargetRealSegIndex.erase(TargetRealSegIndex.begin() + i);
		TargetShapeSegmentIndex.erase(TargetShapeSegmentIndex.begin() + i);
		TargetShapeSegmentAxis.erase(TargetShapeSegmentAxis.begin() + i);
		TargetShapeSegmentAxisDirection.erase(TargetShapeSegmentAxisDirection.begin() + i);
		i--;
		TargetShapeSegmentNum--;
	}

	SourceShapeSegmentJointIndex.resize(SourceShapeSegmentNum);
	SourceShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < sourceVaildSegNum; i++)
		SourceShapeSegmentJointIndex[i] = 1;

	TargetShapeSegmentJointIndex.resize(TargetShapeSegmentNum);
	TargetShapeSegmentJointIndex.fill(-1);
	for (int i = 0; i < targetVaildSegNum; i++)
		TargetShapeSegmentJointIndex[i] = 1;

	SourceSegAdjacencyMatrix = Eigen::MatrixXd::Identity(SourceShapeSegmentNum, SourceShapeSegmentNum);
	TargetSegAdjacencyMatrix = Eigen::MatrixXd::Identity(TargetShapeSegmentNum, TargetShapeSegmentNum);

	ApplySeg(0);
	ApplySeg(1);

	GetSegFaceNum();

	return true;
}

void CorrFinder::ApplySeg(int SorT)
{
	SurfaceMeshModel * proessShape;
	QVector<QVector<int>> SegmentIndex;
	QVector<int> RealSegIndex;
	int SegNum;
	if (SorT == 0)
	{
		proessShape = SourceShape;
		SegNum = SourceShapeSegmentNum;
		SegmentIndex = SourceShapeSegmentIndex;
		RealSegIndex = SourceRealSegIndex;
	}
	else
	{
		proessShape = TargetShape;
		SegNum = TargetShapeSegmentNum;
		SegmentIndex = TargetShapeSegmentIndex;
		RealSegIndex = TargetRealSegIndex;
	}
	Surface_mesh::Face_iterator fit, fend = proessShape->faces_end();
	Surface_mesh::Face_property<QVector<int>> fseg = proessShape->face_property<QVector<int>>("f:seg");
	int findex = 0;
	for (fit = proessShape->faces_begin(); fit != fend; ++fit)
	{
		fseg[fit].clear();
		for (int i = 0; i < SegNum; i++)
		{
			if (SegmentIndex[i][findex] == 1)
			{
				fseg[fit].push_back(i);
			}			
		}
		findex++;
	}
}