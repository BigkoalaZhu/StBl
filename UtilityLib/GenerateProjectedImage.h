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
	GenerateProjectedImage(SurfaceMesh::SurfaceMeshModel* mesh, QString dir, int num);
	~GenerateProjectedImage();

	void projectImage(int index, QString filename);
private:
	void LoadCameras();
	void GenerateProjectedImage::sweepTriangle(CvMat *depthMap, CvMat *labelMap, int label, Eigen::Vector3d *point, Eigen::Vector3i color, IplImage* I);

	SurfaceMesh::SurfaceMeshModel* model;

	double length;
	QString camera_path;
	int camera_num;
	std::vector<Eigen::Vector3d> camera_direction;
	std::vector<Eigen::Vector3d> camera_up;
};

