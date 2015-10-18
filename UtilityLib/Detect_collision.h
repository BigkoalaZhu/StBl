#pragma once
#include "RAPID.h"
#include "SurfaceMeshModel.h"
using namespace SurfaceMesh;

class DetectCollision
{
public:
	DetectCollision(){rapid = NULL;};
	DetectCollision(QVector<SurfaceMeshModel *> components);
	~DetectCollision();

	bool DoDetect(int a,int b);
	std::vector<int> Get_contact_faces(int index);
private:
	RAPID * rapid;
	QVector<SurfaceMeshModel *> components_copied;
	std::vector<RAPID_model*> mdl;
	std::vector<std::vector<int>> contact_faces; 
};