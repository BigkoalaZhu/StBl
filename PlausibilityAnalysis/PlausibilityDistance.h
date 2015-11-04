#pragma once

#include "UtilityGlobal.h"

class PlausibilityDistance
{
public:
	PlausibilityDistance(QString file_path, int cameraNum);
	~PlausibilityDistance();

	void CalculatePairwiseDistance();
	void GenerateBiSHDescriptor();

	//For debug
	double CalculatePairDistance(int index_i, int index_j);

private:
	int instance_number;
	QString FilePath;
	int CameraNum;

	QVector<ShapeNetModelInfo> ShapeNet_Data;
	Eigen::MatrixXd PairwiseEMDistance;
};

