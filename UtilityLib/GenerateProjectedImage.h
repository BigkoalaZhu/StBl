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
	~GenerateProjectedImage();

	void projectImage(int index, QString filename);
private:
	void LoadCameras();
	void GenerateProjectedImage::sweepTriangle(CvMat *depthMap, Eigen::Vector3d *point, IplImage* I);

	SurfaceMesh::SurfaceMeshModel* model;

	double length;
	QString camera_path;
	std::vector<Eigen::Vector3d> camera_direction;
	std::vector<Eigen::Vector3d> camera_up;

	double maxDepth, minDepth;
	QVector< QColor > colormap;
};

