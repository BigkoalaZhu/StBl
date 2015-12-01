#pragma once
#include "SurfaceMeshModel.h"

#include <stack>

enum SymmetryType { TRANSLATION, REFLECTION, ROTATION, INVALID};
struct SymmetryGroup {
	SymmetryType type;
	Eigen::Vector3d axis, delta;
	std::vector<SurfaceMesh::SurfaceMeshModel *> members;
	std::vector<int> labels;
	SymmetryGroup() : type(INVALID), axis(Eigen::Vector3d(0, 0, 0)), delta(Eigen::Vector3d(0, 0, 0)) {}
};

class SymmetryAnalysis
{
public:
	SymmetryAnalysis(SurfaceMesh::SurfaceMeshModel * mesh);
	SymmetryAnalysis(std::vector<SurfaceMesh::SurfaceMeshModel *> meshes);

	void analyze();

	Eigen::MatrixXd centers;
	Eigen::AlignedBox3d boundingVolume;
	Eigen::Vector3d axis, primaryAxis, secondaryAxis, origin;
	bool isFitLine, isRotational, isDiagonalTranslation, isReflecitonal;

	static std::vector< SurfaceMesh::SurfaceMeshModel* > connectedPieces(SurfaceMesh::SurfaceMeshModel * mesh);
	std::vector<SurfaceMesh::SurfaceMeshModel *> meshes;

};

