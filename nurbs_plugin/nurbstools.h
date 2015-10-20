#pragma once

#include <QWidget>

#include "PartGraph.h"
#include "PartCurve.h"
#include "PartSheet.h"

namespace Ui {
class NURBSTools;
}

class nurbs_plugin;

class NURBSTools : public QWidget
{
    Q_OBJECT
    
public:
    explicit NURBSTools(nurbs_plugin * usePlugin, QWidget *parent = 0);
    ~NURBSTools();
    
    nurbs_plugin * plugin;

	int uCount();
	int vCount();

	double resolution();

	int contractIterations();

	bool isRemesh();
	bool isVoxelize();
    bool isConcaveHull();
	bool isUseMedial();
	bool isProtectSmallFeatures();

	double remeshParamter();
	double voxelParamter();

	void correspondTwoSheets(Structure::Sheet *sSheet, Structure::Sheet *tSheet, Structure::Graph * tgt);
	void correspondTwoCurves(Structure::Curve *sCurve, Structure::Curve *tCurve, Structure::Graph * tgt);

public slots:
	void selectionChanged();
	void fillList();
	QStringList selectedGroups();

	void convertToCurve();
	void convertToSheet();

	void modifyGraph();

	void flipU();
	void flipV();

private:
    Ui::NURBSTools *ui;
};
