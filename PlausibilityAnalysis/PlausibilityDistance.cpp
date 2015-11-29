#include "PlausibilityDistance.h"
#include "emdL1.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QTime>
#include "BiSHDist.h"

PlausibilityDistance::PlausibilityDistance(QString file_path, int cameraNum)
{
	FilePath = file_path;
	ShapeNet_Data = ShapeNetFormate::LoadFolder(file_path);
	instance_number = ShapeNet_Data.size();
	PairwiseEMDistance = Eigen::MatrixXd::Zero(instance_number, instance_number);

	CameraNum = cameraNum;
}

PlausibilityDistance::~PlausibilityDistance()
{
	
}

void PlausibilityDistance::GenerateBiSHDescriptor()
{
	QVector<int> errs;
	for (int i = 0; i < instance_number; i++)
	{
        #pragma omp parallel for
		for (int j = 0; j < CameraNum; j++)
		{
			QString filename = ShapeNet_Data[i].FileLocation + "/ProjectedImages/" + QString::number(j) + ".jpg";
			QFileInfo dir(filename);
			QFileInfo dir_s(ShapeNet_Data[i].FileLocation + "/ProjectedImages/" + QString::number(j) + "_s.txt");
			QFileInfo dir_l(ShapeNet_Data[i].FileLocation + "/ProjectedImages/" + QString::number(j) + "_l.txt");

			if (dir_s.exists() && dir_l.exists())
			{
			}
			else if (!dir.exists())
			{
				errs.push_back(i);
			}
			else
			{

				QString outfile_s = ShapeNet_Data[i].FileLocation + "/ProjectedImages/" + QString::number(j) + "_s.txt";
				QString outfile_l = ShapeNet_Data[i].FileLocation + "/ProjectedImages/" + QString::number(j) + "_l.txt";

				BiSHDist::ImageSimlify(filename.toStdString(), outfile_s.toStdString(), 10);
				BiSHDist::ImageSimlify(filename.toStdString(), outfile_l.toStdString(), 20, false);
			}
		}
	}

	std::ofstream out("errs.txt");
	for (int i = 0; i < errs.size(); i++)
		out << errs[i] << " ";
	out.close();
}

void PlausibilityDistance::CalculatePairwiseDistance()
{
	QTime time;
	time.start();
	for (int i = 0; i < instance_number; i++)
	{
		for (int j = i + 1; j < instance_number; j++)
		{
			PairwiseEMDistance(i, j) = CalculatePairDistance(i, j);
			PairwiseEMDistance(j, i) = PairwiseEMDistance(i, j);
		}
	}
	std::ofstream out("distance_l.txt");
	for (int i = 0; i < PairwiseEMDistance.rows(); i++)
	{
		for (int j = 0; j < PairwiseEMDistance.cols(); j++)
		{
			out << PairwiseEMDistance(i, j) << " ";
		}
		out << "\n";
	}
	out.close();
}

double PlausibilityDistance::CalculatePairDistance(int index_i, int index_j)
{
	double Dist = 0;

	int errs = 0;
#pragma omp parallel for
	for (int i = 0; i < CameraNum; i++)
	{
/*		EmdL1 Emdistance;
		Emdistance.SetMaxIteration(1e5);
		cv::Mat imageA, imageB;
		imageA = cv::imread((ShapeNet_Data[index_i].FileLocation + "/ProjectedImages/" + QString::number(i) + ".bmp").toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
		imageB = cv::imread((ShapeNet_Data[index_j].FileLocation + "/ProjectedImages/" + QString::number(i) + ".bmp").toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

		cv::resize(imageA, imageA, cv::Size(32, 32));
		cv::resize(imageB, imageB, cv::Size(32, 32));

		double *input_i = new double[imageA.rows*imageA.cols];
		double *input_j = new double[imageB.rows*imageB.cols];

		double i_total = 0;
		double j_total = 0;
		double ratio_i, ratio_j = 0;

		for (int j = 0; j < imageA.rows*imageA.cols; j++)
		{
			input_i[j] = double(255 - imageA.data[j])/255.0f;
			input_j[j] = double(255 - imageB.data[j])/255.0f;

			i_total += input_i[j];
			j_total += input_j[j];
		}

		ratio_i = 10.0f / i_total;
		ratio_j = 10.0f / j_total;

		for (int j = 0; j < imageA.rows*imageA.cols; j++)
		{
			input_j[i] = input_j[i] * ratio_i;
			input_j[j] = input_j[j] * ratio_j;
		}
		
		double w = ratio_i > ratio_j ? ratio_i : ratio_j;

		Dist += Emdistance.EmdDist(input_i, input_j, imageA.rows, imageA.cols) * w; //May have bugs here*/

		QString A_s, A_l, B_s, B_l;
		A_s = ShapeNet_Data[index_i].FileLocation + "/ProjectedImages/" + QString::number(i) + "_s.txt";
		A_l = ShapeNet_Data[index_i].FileLocation + "/ProjectedImages/" + QString::number(i) + "_l.txt";

		B_s = ShapeNet_Data[index_j].FileLocation + "/ProjectedImages/" + QString::number(i) + "_s.txt";
		B_l = ShapeNet_Data[index_j].FileLocation + "/ProjectedImages/" + QString::number(i) + "_l.txt";

		QFileInfo dir_s(A_l);
		QFileInfo dir_l(B_s);

		if (!dir_s.exists() || !dir_l.exists())
		{
			errs++;
			continue;
		}
		
		float disttmp1 = 0, disttmp2 = 0;
		BiSHDist::ImageMatch(A_l.toStdString(), B_s.toStdString(), disttmp1);
//		BiSHDist::ImageMatch(B_l.toStdString(), A_s.toStdString(), disttmp2);

		Dist += (disttmp1 + disttmp2) / 2;

	}

	Dist = (Dist / (CameraNum - errs)) * CameraNum;

	return Dist;
}

