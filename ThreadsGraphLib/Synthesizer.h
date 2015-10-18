#pragma once

#include "ThreadsGraph.h"
using namespace ThreadsGraph;

#include "RMF.h"

extern int randomCount;
extern int uniformTriCount;

struct ParameterCoord{
	float u, v;
	float theta, psi;

	float origOffset;
	Eigen::Vector3f origPoint;
	Eigen::Vector3f origNormal;
	ThreadsGraph::Node * origNode;

	ParameterCoord(){ u = v = -1; theta = psi = 0; origOffset = 0; origNode = NULL; }
	ParameterCoord(float theta, float psi, float u, float v = 0, float offset = 0, ThreadsGraph::Node * node = NULL){
		this->u = u;
		this->v = v;

		this->theta = theta;
		this->psi = psi;

		this->origOffset = offset;
		this->origNode = node;
	}

	bool operator < (const ParameterCoord& other) const{
		return (u < other.u);
	}
};
static inline QDebug operator<<(QDebug dbg, const ParameterCoord &c){
	dbg.nospace() << QString("[ %1, %2] - theta = %3 \tpsi = %4").arg(c.u).arg(c.v).arg(c.theta).arg(c.psi);
	return dbg.space();
}

typedef QMap<QString, QMap<QString, QVariant> > SynthData;

struct Synthesizer{

	// Generate sample points in the parameter domain
	static QVector<ParameterCoord> genPointCoordsCurve(ThreadsGraph::Curve * curve, const std::vector<Eigen::Vector3f> & points, const std::vector<Eigen::Vector3f> & normals);
	static QVector<ParameterCoord> genPointCoordsSheet(ThreadsGraph::Sheet * sheet, const std::vector<Eigen::Vector3f> & points, const std::vector<Eigen::Vector3f> & normals);

	static QVector<ParameterCoord> genFeatureCoords(ThreadsGraph::Node * node);
	static QVector<ParameterCoord> genEdgeCoords(ThreadsGraph::Node * node);
	static QVector<ParameterCoord> genRandomCoords(ThreadsGraph::Node * node, int samples_count);
	static QVector<ParameterCoord> genUniformCoords(ThreadsGraph::Node * node, float sampling_resolution = -1);
	static QVector<ParameterCoord> genRemeshCoords(ThreadsGraph::Node * node);
	static QVector<ParameterCoord> genUniformTrisCoords(ThreadsGraph::Node * node);

	static QVector<ParameterCoord> genSampleCoordsCurve(ThreadsGraph::Curve * curve, int samplingType = Features | Random);
	static QVector<ParameterCoord> genSampleCoordsSheet(ThreadsGraph::Sheet * sheet, int samplingType = Features | Random);

    // Compute the geometry on given samples in the parameter domain
	static void sampleGeometryCurve(QVector<ParameterCoord> samples, ThreadsGraph::Curve * curve, QVector<float> &offsets, QVector<Vec2f> &normals);
	static void sampleGeometrySheet(QVector<ParameterCoord> samples, ThreadsGraph::Sheet * sheet, QVector<float> &offsets, QVector<Vec2f> &normals);

	// Preparation
	enum SamplingType{ Features = 1, Edges = 2, Random = 4, Uniform = 8, All = 16, AllNonUniform = 32, Remeshing = 64, TriUniform = 128 };

	static void prepareSynthesizeCurve(ThreadsGraph::Curve * curve1, ThreadsGraph::Curve * curve2, int samplingType, SynthData & output);
	static void prepareSynthesizeSheet(ThreadsGraph::Sheet * sheet1, ThreadsGraph::Sheet * sheet2, int samplingType, SynthData & output);
	
	// Blend geometries
	static void blendGeometryCurves(ThreadsGraph::Curve * curve, float alpha, const SynthData & data, QVector<Eigen::Vector3f> &points, QVector<Eigen::Vector3f> &normals, bool isApprox);
	static void blendGeometrySheets(ThreadsGraph::Sheet * sheet, float alpha, const SynthData & data, QVector<Eigen::Vector3f> &points, QVector<Eigen::Vector3f> &normals, bool isApprox);

	// Reconstruction on given base skeleton
	static void reconstructGeometryCurve(ThreadsGraph::Curve * base_curve, const QVector<ParameterCoord> &in_samples, const QVector<float> &in_offsets,
		const QVector<Vec2f> &in_normals, QVector<Eigen::Vector3f> &out_points, QVector<Eigen::Vector3f> &out_normals, bool isApprox);
	static void reconstructGeometrySheet(ThreadsGraph::Sheet * base_sheet, const QVector<ParameterCoord> &in_samples, const QVector<float> &in_offsets,
		const QVector<Vec2f> &in_normals, QVector<Eigen::Vector3f> &out_points, QVector<Eigen::Vector3f> &out_normals, bool isApprox);

	// Blend skeleton bases
	static void blendCurveBases(ThreadsGraph::Curve * curve1, ThreadsGraph::Curve * curve2, float alpha);
	static void blendSheetBases(ThreadsGraph::Sheet * sheet1, ThreadsGraph::Sheet * sheet2, float alpha);

	// Helper functions
	static RMF consistentFrame(ThreadsGraph::Curve * curve, Array1D_Vector4d & coords);

	// IO
	static void saveSynthesisData(ThreadsGraph::Node *node, QString prefix, SynthData & input);
	static int loadSynthesisData(ThreadsGraph::Node *node, QString prefix, SynthData & output);
	static void writeXYZ( QString filename, std::vector<Eigen::Vector3f> points, std::vector<Eigen::Vector3f> normals );
};

Q_DECLARE_METATYPE(Eigen::Vector3f)
Q_DECLARE_METATYPE(QVector<float>)
Q_DECLARE_METATYPE(QVector<Vec2f>)
Q_DECLARE_METATYPE(QVector<Eigen::Vector3f>)
Q_DECLARE_METATYPE(QVector<ParameterCoord>)
