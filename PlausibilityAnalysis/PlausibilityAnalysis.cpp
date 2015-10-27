#include "PlausibilityAnalysis.h"
#include "ShapeNetFormat.h"

template <int DIM = -1>
PlausibilityAnalysis<DIM>::PlausibilityAnalysis(QString filepath, int num)
{
	path = filepath;
	camera_num = num;
	KDtrees.resize(camera_num);
}

template <int DIM = -1>
PlausibilityAnalysis<DIM>::~PlausibilityAnalysis()
{
}

template <int DIM = -1>
void PlausibilityAnalysis<DIM>::processingData(int index)
{
	QVector<ShapeNetModelInfo> ShapeList = ShapeNetFormate::LoadFolder(path);
	cv::HOGDescriptor hog(cv::Size(32, 32), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	for (int i = 0; i < ShapeList.size(); i++)
	{
		QString file = ShapeList[i].FileLocation + "/ProjectedImages/" + QString::number(index) + ".bmp";
		cv::Mat img = cv::imread(file.toStdString().data(), 1);

		vector<float> descriptorsValues;
		hog.compute(img, descriptorsValues);

		for (int k = 0; k < descriptorsValues.size(); k += DIM)
		{
			Eigen::VectorXf feature = Eigen::VectorXf::Zero(DIM);

			for (int n = 0; n < DIM; n++)
				feature[n] = descriptorsValues[k + n];
			KDtrees[index].addPoint(feature);
		}
	}
	KDtrees[index].build();
}
