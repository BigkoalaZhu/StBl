#pragma once
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SurfaceMeshModel.h"
#include "SurfaceMeshHelper.h"
using namespace SurfaceMesh;

class GenerateProjectedImage
{
public:
	GenerateProjectedImage(SurfaceMesh::SurfaceMeshModel* mesh, QString dir);
	GenerateProjectedImage(QString filename, QString dir);
	~GenerateProjectedImage();

	void projectImage(int index, QString filename, int mode = 0);
	int getCameraSize(){ return camera_direction.size(); }
private:
	void LoadCameras();
	void GenerateProjectedImage::sweepTriangle(CvMat *depthMap, QVector<Eigen::Vector3d> point, IplImage* I);

	SurfaceMesh::SurfaceMeshModel* model;
	Eigen::MatrixXd Model_vertex;
	Eigen::MatrixXi Model_face;

	double length;
	QString camera_path;
	std::vector<Eigen::Vector3d> camera_direction;
	std::vector<Eigen::Vector3d> camera_up;

	double maxDepth, minDepth;
	QVector< QColor > colormap;
};

