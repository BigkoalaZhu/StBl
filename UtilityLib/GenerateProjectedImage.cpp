#include "GenerateProjectedImage.h"
#include "Colormap.h"
#include <QFile>
#include <QDir>

GenerateProjectedImage::GenerateProjectedImage(SurfaceMesh::SurfaceMeshModel* mesh, QString dir)
{
	model = mesh;
	camera_path = dir;

	mesh->update_face_normals();
	mesh->update_vertex_normals();
	mesh->updateBoundingBox();

	length = mesh->bbox().diagonal().norm();

	colormap = makeColorMap();
	LoadCameras();


}


GenerateProjectedImage::~GenerateProjectedImage()
{
}

void GenerateProjectedImage::LoadCameras()
{
	int TotalNum = 0;
	QDir dir(camera_path);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);

	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) 
	{
		QFileInfo fileInfo = list.at(i);
		QString name = fileInfo.suffix();
		if (fileInfo.suffix() != "OBJ")
			continue;
		SurfaceMesh::SurfaceMeshModel* mesh = new SurfaceMesh::SurfaceMeshModel;
		mesh->read(fileInfo.absoluteFilePath().toStdString());

		TotalNum += mesh->vertices_size();
		Vector3VertexProperty points = mesh->vertex_property<Vector3>("v:point");

		int index = 0;
		foreach( Vertex v, mesh->vertices() )
		{
			camera_direction.push_back(points[v]);
			Eigen::Vector3d tmp = points[v].cross(Eigen::Vector3d(0, 1, 0));
			camera_up.push_back((tmp.cross(points[v])).normalized());
			index++;
		}
	}
}

void GenerateProjectedImage::projectImage(int index, QString filename)
{
	int v_num = model->vertices_size();
	int f_num = model->faces_size();
	int Width = 256;

	Eigen::MatrixXd tmp = Eigen::MatrixXd::Zero(v_num, 3);
	Eigen::MatrixXd result = Eigen::MatrixXd::Zero(v_num, 3);

	Vector3VertexProperty points = model->vertex_property<Vector3>("v:point");
	Vector3VertexProperty projected = model->add_vertex_property<Vector3>("v:projected");
	

	foreach(Vertex v, model->vertices())
	{
		Eigen::Vector3d vertex = points[v];

		tmp(v.idx(), 0) = (vertex - camera_direction[index] * length).dot(camera_direction[index].cross(camera_up[index]));
		tmp(v.idx(), 1) = (vertex - camera_direction[index] * length).dot(camera_up[index]);
		tmp(v.idx(), 2) = (vertex - camera_direction[index] * length).dot(-camera_direction[index]);
	}

	Eigen::Vector3d boundaryMax, boundaryMin, boundaryL, boundaryC;
	boundaryMax = tmp.colwise().maxCoeff();
	boundaryMin = tmp.colwise().minCoeff();
	boundaryL = boundaryMax - boundaryMin;
	boundaryC = (boundaryMax + boundaryMin) / 2;
	double scale = 1.05*boundaryL.maxCoeff() / Width;
	double offset_x = Width / 2.0f - boundaryC[0] / scale;
	double offset_y = Width / 2.0f - boundaryC[1] / scale;

	int index_j = 0;
	double minz = 9999, maxz = -9999;
	foreach(Vertex v, model->vertices())
	{
		projected[v] = Vector3(tmp(index_j, 0) / scale + offset_x, Width - tmp(index_j, 1) / scale - offset_y, -tmp(index_j, 2) / scale);
		if (minz > projected[v][2])
			minz = projected[v][2];
		if (maxz < projected[v][2])
			maxz = projected[v][2];
		index_j++;
	}
	maxDepth = maxz;
	minDepth = minz;

	Eigen::VectorXi valid = Eigen::VectorXi::Ones(f_num);

	CvMat *depthMap = cvCreateMat(Width, Width, CV_32FC1);
	IplImage* image = cvCreateImage(cvGetSize(depthMap), 8, 3);

	for (int j = 0; j < Width*Width; j++)
	{
		depthMap->data.fl[j] = std::numeric_limits<float>::infinity();
		image->imageData[3 * j] = 255;
		image->imageData[3 * j + 1] = 255;
		image->imageData[3 * j + 2] = 255;
	}

	Surface_mesh::Vertex_around_face_circulator fvit, fvend;
	foreach(Face f, model->faces())
	{		
		Eigen::Vector3d p[3];
		int v_index = 0;
		fvit = fvend = model->vertices(f);
		do
		{
			p[v_index] = projected[fvit];
			v_index++;
		} while (++fvit != fvend);
		sweepTriangle(depthMap, p, image);
	}

	cvSaveImage(filename.toStdString().data(), image);
}

void GenerateProjectedImage::sweepTriangle(CvMat *depthMap, Eigen::Vector3d *point, IplImage* I)
{
	int upMost, downMost;
	bool updated = false;
	int margin_x[2];
	float margin_z[2];
	for (int i = 0; i<3; i++)
	{
		if (!updated)
		{
			updated = true;
			upMost = downMost = point[i][1];
		}
		else
		{
			upMost = (point[i][1] < upMost) ? point[i][1] : upMost;
			downMost = (point[i][1] > downMost) ? point[i][1] : downMost;
		}
	}
	for (int y = upMost; y <= downMost; y++)
	{
		int mIdx = 0;
		for (int i = 0; i<3; i++)
		{
			Eigen::Vector3d &p1 = point[i];
			Eigen::Vector3d &p2 = point[(i + 1) % 3];
			float dy1 = p1[1] - y;
			float dy2 = p2[1] - y;
			if (dy1*dy2 <= 0)
			{
				margin_x[mIdx] = (dy2 - dy1 == 0) ? p1[0] : p1[0] - dy1 / (dy2 - dy1)*(p2[0] - p1[0]);
				margin_z[mIdx] = (dy2 - dy1 == 0) ? p1[2] : p1[2] - dy1 / (dy2 - dy1)*(p2[2] - p1[2]);
				mIdx++;
			}
		}
		if (mIdx != 2)
		{
			continue;
		}
		if (margin_x[0] == margin_x[1])
		{
			if (depthMap->data.fl[y*depthMap->width + margin_x[0]] > margin_z[0])
			{
				//QColor color = getColorFromMap((margin_z[0] - minDepth)/(maxDepth - minDepth), colormap);
				int d = 255 * (margin_z[0] - minDepth) / (maxDepth - minDepth);
				QColor color(d, d, d);

				depthMap->data.fl[y*depthMap->width + margin_x[0]] = margin_z[0];
				I->imageData[y*I->widthStep + 3 * margin_x[0]] = color.red();
				I->imageData[y*I->widthStep + 3 * margin_x[0] + 1] = color.green();
				I->imageData[y*I->widthStep + 3 * margin_x[0] + 2] = color.blue();
			}
			continue;
		}
		int mx_s = (margin_x[0] < margin_x[1]) ? margin_x[0] : margin_x[1];
		int mx_e = (margin_x[0] >= margin_x[1]) ? margin_x[0] : margin_x[1];
		for (int x = mx_s; x <= mx_e; x++)
		{
			float z = margin_z[0] + float(x - margin_x[0]) / (margin_x[1] - margin_x[0])*(margin_z[1] - margin_z[0]);
			if (depthMap->data.fl[y*depthMap->width + x] > z)
			{
				//QColor color = getColorFromMap((z - minDepth) / (maxDepth - minDepth), colormap);
				int d = 255 * (z - minDepth) / (maxDepth - minDepth);
				QColor color(d, d, d);

				depthMap->data.fl[y*depthMap->width + x] = z;
				I->imageData[y*I->widthStep + 3 * x] = color.red();
				I->imageData[y*I->widthStep + 3 * x + 1] = color.green();
				I->imageData[y*I->widthStep + 3 * x + 2] = color.blue();
			}
		}
	}
}