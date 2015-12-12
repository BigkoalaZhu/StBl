#include "CorrespondenceEvaluate.h"
#include "TopoBlender.h"
#include "Scheduler.h"
#include "SynthesisManager.h"

CorrespondenceEvaluate::~CorrespondenceEvaluate()
{
}

void CorrespondenceEvaluate::initialGraph()
{
	SourceGraph = new Structure::Graph(SourceFile, 0);
	TargetGraph = new Structure::Graph(TargetFile, 0);
	GraphCorr = new GraphCorresponder(SourceGraph, TargetGraph);
}

void CorrespondenceEvaluate::assignCorr(QVector<QString> sParts, QVector<QString> tParts)
{
	GraphCorr->addCorrespondences(sParts, tParts, -1.0f);
}

void CorrespondenceEvaluate::generateInbetween(double t)
{
	auto scheduler = QSharedPointer<Scheduler>(new Scheduler);
	auto blender = QSharedPointer<TopoBlender>(new TopoBlender(GraphCorr, scheduler.data()));
	auto synthManager = QSharedPointer<SynthesisManager>(new SynthesisManager(GraphCorr, scheduler.data(), blender.data(), 1000));
	synthManager->genSynData();
	scheduler->timeStep = 1.0 / 100.0;
	scheduler->defaultSchedule();
	scheduler->executeAll();
	auto blendedModel = scheduler->allGraphs[t * (scheduler->allGraphs.size() - 1)];
	synthManager->renderGraph(*blendedModel, "test", false, 5);
}
