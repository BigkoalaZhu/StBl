#pragma once

#include <QtCore>
#include "GraphCorresponder.h"

class CorrespondenceEvaluate
{
public:
	CorrespondenceEvaluate(QString Source, QString Target){ SourceFile = Source; TargetFile = Target; }
	~CorrespondenceEvaluate();

	void initialGraph();
	void assignCorr(QVector<QString> sParts, QVector<QString> tParts);
	void generateInbetween(double t);

private:
	QString SourceFile;
	QString TargetFile;

	Structure::Graph *SourceGraph;
	Structure::Graph *TargetGraph;
	GraphCorresponder *GraphCorr;
};

