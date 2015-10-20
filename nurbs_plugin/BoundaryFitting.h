#pragma once

#include "SurfaceMeshModel.h"
#include "FaceBarycenterHelper.h"

#include "NanoKdTree.h"
//#include "../CustomDrawObjects.h"

typedef QPair<Halfedge,double> HalfedgeTime;
typedef QMap<double, HalfedgeTime > DistanceHalfedge;

struct BoundaryFitting{

    BoundaryFitting( SurfaceMeshModel * mesh = NULL, int numSegments = 20, int numSegmentsV = -1);
	void doFit();

	std::vector<SurfaceMesh::Vertex> collectRings( SurfaceMesh::Vertex v, size_t min_nb, bool isBoundary = false );
	
	QVector<SurfaceMesh::Vertex> boundaryVerts();

	void normalize( SurfaceMesh::ScalarVertexProperty & vprop );

	void gradientFaces( ScalarVertexProperty & functionVal, bool isSmooth = true, bool isNormalizeNegateGradient = true );
	QVector<SurfaceMesh::Vertex> neighbours( int start, int range, QVector<SurfaceMesh::Vertex> all );

	SurfaceMesh::Halfedge get_halfedge(SurfaceMesh::Vertex start, SurfaceMesh::Vertex end);
    SurfaceMesh::Halfedge getBestEdge(Eigen::Vector3d prevPoint, Eigen::Vector3d direction, Face f, double & bestTime);
    SurfaceMesh::Face getBestFace( Eigen::Vector3d & point, Vertex guess );

    std::vector<Eigen::Vector3d> geodesicPath(Eigen::Vector3d fromPoint, Eigen::Vector3d toPoint);
    std::vector< std::vector<Eigen::Vector3d> > geodesicPaths( std::vector<Eigen::Vector3d> fromPoints, std::vector<Eigen::Vector3d> toPoints, int segments = -1 );

    std::vector<Eigen::Vector3d> trianglePoints( Face f );
	std::vector<Vertex> triangleVertices( Face f );

	SurfaceMesh::SurfaceMeshModel * part;
	int segments, segmentsV;

	SurfaceMesh::ScalarVertexProperty vals;
	SurfaceMesh::ScalarVertexProperty boundaryCurv;
	SurfaceMesh::ScalarVertexProperty dists;
	SurfaceMesh::Vector3VertexProperty vgradient;

	SurfaceMesh::Vector3FaceProperty fgradient;
	SurfaceMesh::Vector3VertexProperty vdirection;
	SurfaceMesh::Vector3VertexProperty normals;
	SurfaceMesh::Vector3VertexProperty points;

	NanoKdTree tree;

	// output
    std::vector< std::vector<Eigen::Vector3d> > lines;

    std::vector<Eigen::Vector3d> debugPoints, debugPoints2;
    std::vector<Eigen::Vector3d> corners;
    std::vector< std::vector<Eigen::Vector3d> > main_edges;
    //LineSegments ls;
};
