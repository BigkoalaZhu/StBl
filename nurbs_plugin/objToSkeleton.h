#pragma once

#include "NURBSCurve.h"
#include "NURBSRectangle.h"
#include "GenericGraph.h"

#include "SurfaceMeshModel.h"
using namespace SurfaceMesh;
#include "SamplingHelper.h"

#include "PowerCrustSurfaceReconstruction.h"
#include "OrientHelper.h"

#include "PluginManager.h"
#include "RichParameterSet.h"
#include "FilterPlugin.h"

#include "StarlabApplication.h"

#define NO_OPENGL
#include "OBB_Volume.h"

struct nurbs_plugin{

    Starlab::Application * app;

    double m_voxelParamter, m_remeshParamter;

    nurbs_plugin(QString filename, bool isManifold = true, bool isUseBoxes = false, bool isConcaveHull = false, 
		bool isVoxelize = false, bool isRemesh = false, bool isUseMedial = false, 
		bool isProtectSmallFeatures = false, double VoxelParam = 0.05, double RemeshParam = 0.02) : mcf_params(nullptr),
		m_isUseBoxes(isUseBoxes), m_isConcaveHull(isConcaveHull), m_isVoxelize(isVoxelize), m_isRemesh(isRemesh),
		m_isUseMedial(isUseMedial), m_isProtectSmallFeatures(isProtectSmallFeatures),
		m_voxelParamter(VoxelParam), m_remeshParamter(RemeshParam)
    {
        m = new SurfaceMeshModel("input.obj","input");

        if(isManifold)
        {
            m->read(filename.toStdString());
        }
        else
        {

        }

        app = new Starlab::Application;
        app->document()->addModel(m);
        app->document()->setSelectedModel(m);
    }

    SurfaceMeshModel * m;

    Starlab::PluginManager * pluginManager(){ return app->pluginManager(); }

    bool m_isUseBoxes, m_isConcaveHull, m_isVoxelize, m_isRemesh, m_isUseMedial, m_isProtectSmallFeatures;

    bool isConcaveHull()
    {
        return m_isConcaveHull;
    }

    bool isVoxelize()
    {
        return m_isVoxelize;
    }

    bool isRemesh()
    {
        return m_isRemesh;
    }

    bool isUseMedial()
    {
        return m_isUseMedial;
    }

    bool isProtectSmallFeatures()
    {
        return m_isProtectSmallFeatures;
    }

    double voxelParamter(){ return m_voxelParamter; }
    double remeshParamter(){ return m_remeshParamter; }

	static std::vector<Eigen::Vector3d> refineByResolution(const std::vector<Eigen::Vector3d> & fromPnts, double resolution)
	{
		std::vector<Eigen::Vector3d> resampled_polyline;

		// Parameterize line by finding segments
		double totalLineLength = 0.0;

		typedef std::pair<double, int> LengthSegment;
		std::vector< LengthSegment > segments;

		// Add a start segment
		segments.push_back(LengthSegment(totalLineLength, 1));

		for (int i = 1; i < (int)fromPnts.size(); i++)
		{
			double curLen = (fromPnts[i] - fromPnts[i - 1]).norm();
			totalLineLength += curLen;
			segments.push_back(LengthSegment(totalLineLength, i));
		}

		if (totalLineLength == 0.0)
			return resampled_polyline;

		double numSegments = totalLineLength / resolution;

		// Re-sample line
		for (int i = 0; i < numSegments; i++)
		{
			double t = totalLineLength * (double(i) / (numSegments - 1));

			std::vector< LengthSegment >::iterator it = lower_bound(segments.begin(),
				segments.end(), LengthSegment(qMin(t, totalLineLength), -1));

			// Find start
			int idx = it->second;

			double seg_range = segments[idx].first - segments[idx - 1].first;
			double seg_start = segments[idx - 1].first;

			double alpha = (t - seg_start) / seg_range;

			resampled_polyline.push_back(((1 - alpha) * fromPnts[idx - 1]) + (alpha * fromPnts[idx]));
		}

		return resampled_polyline;
	}

	NURBS::NURBSCurved curveFit(SurfaceMeshModel * part)
	{
		NURBS::NURBSCurved fittedCurve;

		Vector3VertexProperty partPoints = part->vertex_property<Vector3>("v:point");

		GenericGraphs::Graph<int, double> g;
		SurfaceMeshHelper h(part);
		ScalarEdgeProperty elen = h.computeEdgeLengths();

		foreach(Edge e, part->edges()){
			Vertex v0 = part->vertex(e, 0);
			Vertex v1 = part->vertex(e, 1);
			g.AddEdge(v0.idx(), v1.idx(), elen[e]);
		}

		// Find initial furthest point
		g.DijkstraComputePaths(0);
		double max_dist = -DBL_MAX;
		int idxA = 0;
		for (int i = 0; i < (int)part->n_vertices(); i++){
			if (g.min_distance[i] > max_dist){
				max_dist = qMax(max_dist, g.min_distance[i]);
				idxA = i;
			}
		}

		// Find two furthest points
		g.DijkstraComputePaths(idxA);
		max_dist = -DBL_MAX;
		int idxB = 0;
		for (int i = 0; i < (int)part->n_vertices(); i++){
			if (g.min_distance[i] > max_dist){
				max_dist = qMax(max_dist, g.min_distance[i]);
				idxB = i;
			}
		}

		std::list<int> path = g.DijkstraShortestPath(idxA, idxB);

		// Check for loop case
		double r = 0.025 * part->bbox().diagonal().norm();
		QVector<int> pathA, pathB;
		foreach(int vi, path) pathA.push_back(vi);
		Vertex centerPath(pathA[pathA.size() / 2]);

		foreach(Face f, part->faces()){
			QVector<Vertex> vidx;
			Surface_mesh::Vertex_around_face_circulator vit = part->vertices(Face(f)), vend = vit;
			do{ vidx.push_back(vit); } while (++vit != vend);
			Vector3d cp(0, 0, 0);
			if (TestSphereTriangle(partPoints[centerPath], r, partPoints[vidx[0]], partPoints[vidx[1]], partPoints[vidx[2]], cp)){
				int v0 = vidx[0].idx();
				int v1 = vidx[1].idx();
				int v2 = vidx[2].idx();
				g.SetEdgeWeight(v0, v1, DBL_MAX);
				g.SetEdgeWeight(v1, v2, DBL_MAX);
				g.SetEdgeWeight(v2, v0, DBL_MAX);
			}
		}

		foreach(int vi, g.DijkstraShortestPath(idxB, idxA)) pathB.push_back(vi);

		QVector<int> finalPath = pathA;

		// We have a loop
		if (pathB.size() > 0.1 * pathA.size())
			finalPath += pathB;

		std::vector<Vector3d> polyLine;
		for (int i = 0; i < (int)finalPath.size(); i++)
			polyLine.push_back(partPoints[Vertex(finalPath[i])]);

		polyLine = refineByResolution(polyLine, r);
		//polyLine = smoothPolyline(polyLine, 1);

		return NURBS::NURBSCurved::createCurveFromPoints(polyLine);
	}

	double minAngle(Face f, SurfaceMeshModel * ofMesh)
	{
		#undef max
		#define qRanged(min, v, max) ( qMax(min, qMin(v, max)) )

		double minAngle(DBL_MAX);
		Vector3VertexProperty pts = ofMesh->vertex_property<Vector3>("v:point");

		SurfaceMesh::Model::Halfedge_around_face_circulator h(ofMesh, f), eend = h;
		do{
			Vector3 a = pts[ofMesh->to_vertex(h)];
			Vector3 b = pts[ofMesh->from_vertex(h)];
			Vector3 c = pts[ofMesh->to_vertex(ofMesh->next_halfedge(h))];

			double d = dot((b - a).normalized(), (c - a).normalized());
			double angle = acos(qRanged(-1.0, d, 1.0));

			minAngle = qMin(angle, minAngle);
		} while (++h != eend);

		return minAngle;
	}

	NURBS::NURBSRectangled toSheet()
	{
		Vector3 c1(-0.5, -0.5, 0), c2(0.5, 0.5, 0);

		if (m_isUseBoxes)
		{
			auto minobb = OBB_Volume(m);

			int minAxis = 0;
			if (minobb.sides[1] < minobb.sides[minAxis]) minAxis = 1;
			if (minobb.sides[2] < minobb.sides[minAxis]) minAxis = 2;

			auto shortestAxis = minobb.axis().at(minAxis);

			Vector3 delta = shortestAxis * (minobb.sides[minAxis] * 0.5);

			c1 = minobb.corners()[6] + delta;
			c2 = minobb.corners()[0] - delta;
		}
		else
		{
			m->updateBoundingBox();
			auto box = m->bbox();

			int minAxis = 0;
			if (box.sizes()[1] < box.sizes()[minAxis]) minAxis = 1;
			if (box.sizes()[2] < box.sizes()[minAxis]) minAxis = 2;

			Vector3 shortestAxis = box.sizes()[minAxis] * Vector3(minAxis == 0 ? 1 : 0, minAxis == 1 ? 1 : 0, minAxis == 2 ? 1 : 0);

			Vector3 delta = shortestAxis * 0.5;

			c1 = box.min() + delta;
			c2 = box.max() - delta;
		}

		Vector3 diag = c2 - c1;
		Vector3 oldC2 = c2;
		c2 = c1 + (diag * 0.9);
		c1 = oldC2 - (diag * 0.9);

		return NURBS::NURBSRectangled::createSheet(c1,c2);
	}

	NURBS::NURBSCurved toCurve()
	{
		if (m_isUseBoxes)
		{
			auto minobb = OBB_Volume(m);

			int maxAxis = 0;
			if (minobb.sides[1] > minobb.sides[maxAxis]) maxAxis = 1;
			if (minobb.sides[2] > minobb.sides[maxAxis]) maxAxis = 2;

			auto longestAxis = minobb.axis().at(maxAxis);
			Vector3 center = minobb.center();

			Vector3 delta = longestAxis * (minobb.sides[maxAxis] * 0.5);

			delta *= 0.9;

			return NURBS::NURBSCurved::createCurve(center - delta, center + delta);
		}

		#define RADIANS(deg)    ((deg)/180.0 * M_PI)

		prepareSkeletonize();

		double theta = 15.0; // degrees

		for (int i = 0; i < 30; i++){
			stepSkeletonizeMesh();

			// Decide weather or not to keep contracting based on angle of faces
			bool isDone = true;
			foreach(Face f, m->faces()){
				if (minAngle(f, m) > RADIANS(theta))
					isDone = false;
			}

			if (isDone) break;
		}

		// Clear parameters
		mcf_params = NULL;

		foreach(Vertex v, m->vertices()) if (m->is_isolated(v)) m->remove_vertex(v);
		m->garbage_collection();

		// For debugging
		if (isConcaveHull() || isRemesh() || isVoxelize()) m->write("skeletonized.off");

		return curveFit(m);
	}

    void prepareSkeletonize()
    {
        m->update_face_normals();
        m->update_vertex_normals();
        m->updateBoundingBox();

        // Concavehull
        if( isConcaveHull() )
        {
            QVector<Vector3> normals;

            QVector<Vector3> point_cloud;
            //for(auto v : m->vertices()) point_cloud.push_back(m->vertex_coordinates()[v]);
            for(auto v : SamplingHelper(m).similarSampling(500, normals)) point_cloud.push_back(v);

            PowerCrustSurfaceReconstructionImpl pc;
            pc.pcInit();
            for(auto p : point_cloud) pc.input.InsertNextPoint(p[0], p[1], p[2]);
            pc.adapted_main();

            SurfaceMesh::SurfaceMeshModel * m2 = new SurfaceMeshModel("m2.off","m2");
            for(auto p : pc.output.GetPoints()) m2->add_vertex(Point(p[0], p[1], p[2]));

            // Orient and add faces
            OrientHelper ori;
            for (auto f : ori.reorient(pc.output.GetFaces(), m2->n_vertices()))
            {
                std::vector<SurfaceMesh::Vertex> verts;
                for(auto vi : f) verts.push_back(SurfaceMesh::Vertex(vi));
                m2->add_face(verts);
            }

            // Turn to pure triangles
            m2->triangulate();

            // Copy back geometry
            {
                m->clear();
                for (auto v : m2->vertices())
                {
                    auto p = m2->vertex_coordinates()[v];
                    m->add_vertex(p);
                }

                for(auto f : m2->faces()) {
                    std::vector<SurfaceMesh::Vertex> verts;
                    for(auto v : m2->vertices(f)) verts.push_back(v);
                    m->add_face(verts);
                }

                m->update_face_normals();
                m->update_vertex_normals();
                m->updateBoundingBox();
            }
        }

        // Voxelize
        if( isVoxelize() )
        {
            FilterPlugin * voxResamplePlugin = pluginManager()->getFilter("Voxel Resampler");
            RichParameterSet * resample_params = new RichParameterSet;
            voxResamplePlugin->initParameters( resample_params );
            resample_params->setValue("voxel_scale", float( voxelParamter() ));
            voxResamplePlugin->applyFilter( resample_params );
        }

        // Remesh
        if( isRemesh() )
        {
            FilterPlugin * remeshPlugin = pluginManager()->getFilter("Isotropic Remesher");
            RichParameterSet * remesh_params = new RichParameterSet;
            remeshPlugin->initParameters( remesh_params );
            float edge_threshold = float(remeshParamter() * m->bbox().diagonal().norm());

            if(isProtectSmallFeatures()){
                remesh_params->setValue("keep_shortedges", true);
                edge_threshold *= 0.5;
            }

            remesh_params->setValue("edgelength_TH", edge_threshold);

            remeshPlugin->applyFilter( remesh_params );
        }

        m->update_face_normals();
        m->update_vertex_normals();
        m->updateBoundingBox();

		// For debugging
		if (isConcaveHull() || isRemesh() || isVoxelize()) m->write("remeshed.off");

        // Compute MAT
        FilterPlugin * matPlugin = pluginManager()->getFilter("Voronoi based MAT");
        RichParameterSet * mat_params = new RichParameterSet;
        matPlugin->initParameters( mat_params );
        matPlugin->applyFilter( mat_params );
    }

    RichParameterSet * mcf_params;

    void stepSkeletonizeMesh()
    {
        FilterPlugin * mcfPlugin = pluginManager()->getFilter("MCF Skeletonization");

        if(!mcf_params){
            mcf_params = new RichParameterSet;
            mcfPlugin->initParameters( mcf_params );

            // Custom MCF parameters
            if( !isUseMedial() )
                mcf_params->setValue("omega_P_0", 0.0f);
        }
        mcfPlugin->applyFilter( mcf_params );

    }
};
