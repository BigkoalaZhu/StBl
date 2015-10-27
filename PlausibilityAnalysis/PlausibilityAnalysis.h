#pragma once

#include <QString>
#include <QVector>
#include "FeatureSearchingTree.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/objdetect.hpp>


template <int DIM>
class PlausibilityAnalysis
{
public:
	PlausibilityAnalysis(QString filepath, int num);
	~PlausibilityAnalysis();

	void processingData(int index);

private:
	QString path;
	int camera_num;
	QVector<FeatureSearchingTree<DIM>> KDtrees;
};

