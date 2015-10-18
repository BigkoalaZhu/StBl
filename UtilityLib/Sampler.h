#pragma once

#include "SurfaceMeshModel.h"
#include "SurfaceMeshHelper.h"
#include "FaceBarycenterHelper.h"

using namespace SurfaceMesh;

// Helper structures
struct SamplePoint{
	Eigen::Vector3d pos, n;
	double weight;
	double u,v;
	int findex; // index of sampled face
	int flag;

	SamplePoint(const Eigen::Vector3d& position = Eigen::Vector3d(), const Eigen::Vector3d& normal = Eigen::Vector3d(), 
		double Weight = 0.0, int face_index = -1.0, double U = 0.0, double V = 0.0, int flags = 0)
	{
		pos = position;
		n = normal;
		weight = Weight;
		findex = face_index;
		u = U;
		v = V;
		flag = flags;
	}
};

struct WeightFace{
	double weight;
	Surface_mesh::Face f;

    WeightFace(double a = 0.0, Surface_mesh::Face face = Surface_mesh::Face()) : weight(a), f(face){}

	bool operator< (const WeightFace & af) const { return weight < af.weight; }
	void setValue (double val) { weight = val; }
};

enum SamplingMethod {  RANDOM_BARYCENTRIC_AREA,  RANDOM_BARYCENTRIC_WEIGHTED, FACE_CENTER_RANDOM, FACE_CENTER_ALL };

// Class definition
class Sampler{

private:

public:
    Sampler(SurfaceMesh::Model * srcMesh = NULL, SamplingMethod samplingMethod = RANDOM_BARYCENTRIC_AREA );
	Sampler(void * srcMesh, SamplingMethod samplingMethod);
	
	// Get samples
	SamplePoint getSample(double weight = 0.0);
    QVector<SamplePoint> getSamples(int numberSamples, double weight = 0.0);

    SurfaceMesh::Model * mesh;
	SamplingMethod method;

	// For Monte Carlo
    std::vector<WeightFace> interval;
   
	ScalarFaceProperty farea;
    Vector3FaceProperty fnormal;
	Vector3FaceProperty fcenter;
    Vector3VertexProperty points;

    Vector3 getBaryFace(Surface_mesh::Face f, double U, double V);
};

// Helper functions
double inline uniform(double a = 0.0, double b = 1.0){
    double len = b - a;
    return ((double)rand()/RAND_MAX) * len + a;
}

static inline void RandomBaricentric(double * interp){
	interp[1] = uniform();
	interp[2] = uniform();

	if(interp[1] + interp[2] > 1.0)
	{
		interp[1] = 1.0 - interp[1];
		interp[2] = 1.0 - interp[2];
	}

	interp[0] = 1.0 - (interp[1] + interp[2]);
}
