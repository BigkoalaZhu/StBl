#include "nurbstools.h"
#include "ui_nurbstools.h"
#include "nurbs_plugin.h"

#include "GraphModifyWidget.h"

NURBSTools::NURBSTools(nurbs_plugin * usePlugin, QWidget *parent) : QWidget(parent), ui(new Ui::NURBSTools)
{
    ui->setupUi(this);
    this->plugin = usePlugin;

    // Connect
    //plugin->connect(ui->fitCurveButton, SIGNAL(clicked()), SLOT(doFitCurve()));
    //plugin->connect(ui->fitSurfaceButton, SIGNAL(clicked()), SLOT(doFitSurface()));

	plugin->connect(ui->clearButton, SIGNAL(clicked()), SLOT(clearAll()));
	plugin->connect(ui->saveButton, SIGNAL(clicked()), SLOT(saveAll()));
	plugin->connect(ui->loadButton, SIGNAL(clicked()), SLOT(loadGraph()));

	//plugin->connect(ui->skeletonButton, SIGNAL(clicked()), SLOT(skeletonizeMesh()));
	plugin->connect(ui->skeletonButtonStep, SIGNAL(clicked()), SLOT(stepSkeletonizeMesh()));

	this->connect(ui->curveFitButton, SIGNAL(clicked()), SLOT(convertToCurve()));
	this->connect(ui->sheetFitButton, SIGNAL(clicked()), SLOT(convertToSheet()));

	this->connect(ui->flipUButton, SIGNAL(clicked()), SLOT(flipU()));
	this->connect(ui->flipVButton, SIGNAL(clicked()), SLOT(flipV()));

	ui->partsList->connect(ui->clearSelectedButton, SIGNAL(clicked()), SLOT(clearSelection()));
	this->connect(ui->partsList, SIGNAL(itemSelectionChanged()), SLOT(selectionChanged()));
	this->connect(ui->linksButton, SIGNAL(clicked()), SLOT(modifyGraph()));
}

NURBSTools::~NURBSTools()
{
    delete ui;
}

int NURBSTools::uCount()
{
	return ui->uCount->value();
}

int NURBSTools::vCount()
{
	return ui->vCount->value();
}

double NURBSTools::resolution()
{
	return ui->resolution->value();
}

int NURBSTools::contractIterations()
{
	return ui->contractIterations->value();
}

bool NURBSTools::isRemesh()
{
	return ui->remeshOption->isChecked();
}

bool NURBSTools::isVoxelize()
{
	return ui->voxelOption->isChecked();
}

bool NURBSTools::isConcaveHull()
{
    return ui->concaveHullOption->isChecked();
}

bool NURBSTools::isUseMedial()
{
	return ui->medialOption->isChecked();
}

bool NURBSTools::isProtectSmallFeatures()
{
	return ui->protectSmallOption->isChecked();
}

double NURBSTools::remeshParamter()
{
	return ui->remeshParamter->value();
}

double NURBSTools::voxelParamter()
{
	return ui->voxelParamter->value();
}

void NURBSTools::flipU()
{
	QStringList selected = selectedGroups();

	foreach(QString gid, selectedGroups()){
		Structure::Node * n = plugin->graph->getNode(gid);
		if(!n) continue;
		if(n->type() == Structure::CURVE){
			Array1D_Vector3 cpts = n->controlPoints();
			std::reverse(cpts.begin(), cpts.end());
			n->setControlPoints(cpts);
		}
		else{
			Structure::Sheet * sheet = ((Structure::Sheet*)n);
			Array2D_Vector3 cpts = sheet->surface.mCtrlPoint, newPts = cpts;
			int nU = cpts.size(); int nV = cpts.front().size();
			for(int u = 0; u < nU; u++)
				for(int v = 0; v < nV; v++)
					newPts[u][v] = cpts[(nU - 1) - u][v];
			sheet->surface.mCtrlPoint = newPts;
			sheet->surface.quads.clear();
		}
	}

	ui->partsList->clearSelection();
	plugin->updateDrawArea();
}

void NURBSTools::flipV()
{
	QStringList selected = selectedGroups();

	foreach(QString gid, selectedGroups()){
		Structure::Node * n = plugin->graph->getNode(gid);
		if(!n) continue;
		if(n->type() == Structure::CURVE){
			Array1D_Vector3 cpts = n->controlPoints();
			std::reverse(cpts.begin(), cpts.end());
			n->setControlPoints(cpts);
		}
		else{
			Structure::Sheet * sheet = ((Structure::Sheet*)n);
			Array2D_Vector3 cpts = sheet->surface.mCtrlPoint, newPts = cpts;
			int nU = cpts.size(); int nV = cpts.front().size();
			for(int u = 0; u < nU; u++)
				for(int v = 0; v < nV; v++)
					newPts[u][v] = cpts[u][(nV - 1) - v];
			sheet->surface.mCtrlPoint = newPts;
			sheet->surface.quads.clear();
		}
	}

	ui->partsList->clearSelection();
	plugin->updateDrawArea();
}

void NURBSTools::fillList()
{
	ui->partsList->clear();

	foreach(QString groupID, plugin->groupFaces.keys()){
		QListWidgetItem * item = new QListWidgetItem( groupID );
		
		if(plugin->graph->getNode(groupID)) item->setTextColor(Qt::gray);

		ui->partsList->addItem(item);
	}
}

QStringList NURBSTools::selectedGroups()
{
	QStringList selected;
	foreach( QListWidgetItem *item, ui->partsList->selectedItems()){
		selected << item->text();
	}
	return selected;
}

void NURBSTools::selectionChanged()
{
	// Draw selection
	{
		plugin->ps.clear();

		foreach(QString gid, selectedGroups()){
			QVector<int> part = plugin->groupFaces[gid];

			foreach(int fidx, part)
			{
				// Collect points
                QVector<starlab::QVector3> pnts;
				Surface_mesh::Vertex_around_face_circulator vit = plugin->entireMesh->vertices(Face(fidx)),vend=vit;
				do{ pnts.push_back(plugin->entirePoints[vit]); } while(++vit != vend);

				// Add triangle
				plugin->ps.addPoly(pnts, QColor(255,0,0,100));
			}
		}
	}

	plugin->updateDrawArea();
}

void NURBSTools::correspondTwoCurves(Structure::Curve *sCurve, Structure::Curve *tCurve, Structure::Graph * tgt)
{
	std::vector<Vector3> sCtrlPoint = sCurve->controlPoints();
	std::vector<Vector3> tCtrlPoint = tCurve->controlPoints();

	// Euclidean for now, can use Geodesic distance instead if need
	Vector3 scenter = sCurve->center();
	Vector3 sfront = sCtrlPoint.front() - scenter;
	Vector3 tcenter = tCurve->center();
	Vector3 tfront = tCtrlPoint.front() - tcenter;
	Vector3 tback = tCtrlPoint.back() - tcenter;

	float f2f = (sfront - tfront).norm();
	float f2b = (sfront - tback).norm();

	float diff = std::abs(f2f - f2b);
	float threshold = 0.1f;

	if (f2f > f2b && diff > threshold)
	{
		// Flip the target
		std::vector<Scalar> tCtrlWeight = tCurve->controlWeights();
		std::reverse(tCtrlPoint.begin(), tCtrlPoint.end());
		std::reverse(tCtrlWeight.begin(), tCtrlWeight.end());

		NURBS::NURBSCurved newCurve(tCtrlPoint, tCtrlWeight);
		tCurve->curve = newCurve;

		// Update the coordinates of links
		foreach(Structure::Link * l, tgt->getEdges(tCurve->id))
		{
			l->setCoord(tCurve->id, inverseCoords(l->getCoord(tCurve->id)));
		}
	}
}

void NURBSTools::correspondTwoSheets(Structure::Sheet *sSheet, Structure::Sheet *tSheet, Structure::Graph * tgt)
{
	// Old properties
	NURBS::NURBSRectangled &oldRect = tSheet->surface;
	int uDegree = oldRect.GetDegree(0);
	int vDegree = oldRect.GetDegree(1);
	bool uLoop = oldRect.IsLoop(0);
	bool vLoop = oldRect.IsLoop(1);
	bool uOpen = oldRect.IsOpen(0);
	bool vOpen = oldRect.IsOpen(1);
	bool isModified = false;
	bool isUVFlipped = false;

	// Control points and weights
	Array2D_Vector3 sCtrlPoint = sSheet->surface.mCtrlPoint;
	Array2D_Real sCtrlWeight = sSheet->surface.mCtrlWeight;

	Array2D_Vector3 tCtrlPoint = tSheet->surface.mCtrlPoint;
	Array2D_Real tCtrlWeight = tSheet->surface.mCtrlWeight;

	Array2D_Vector3 tCtrlPointNew;
	Array2D_Real tCtrlWeightNew;

	Vector3 scenter = sSheet->center();
	Vector3 tcenter = tSheet->center();

	// Get the extreme points.
	Vector3 s00 = sCtrlPoint.front().front();
	Vector3 s01 = sCtrlPoint.front().back();
	Vector3 s10 = sCtrlPoint.back().front();
	Vector3 sU = s10 - s00;
	Vector3 sV = s01 - s00;

	Vector3 t00 = tCtrlPoint.front().front();
	Vector3 t01 = tCtrlPoint.front().back();
	Vector3 t10 = tCtrlPoint.back().front();
	Vector3 tU = t10 - t00;
	Vector3 tV = t01 - t00;

	// Flip if need
	Vector3 sUV = cross(sU, sV);
	Vector3 tUV = cross(tU, tV);
	if (dot(sUV, tUV) < 0)
	{
		// Reverse the target along u direction
		std::reverse(tCtrlPoint.begin(), tCtrlPoint.end());
		std::reverse(tCtrlWeight.begin(), tCtrlWeight.end());

		// Update tU
		tU = -tU;
		tUV = -tUV;
		isModified = true;

		// Update the coordinates of links
		foreach(Structure::Link * l, tgt->getEdges(tSheet->id)){
			Array1D_Vector4d oldCoord = l->getCoord(tSheet->id), newCoord;
			foreach(Vector4d c, oldCoord) newCoord.push_back(Vector4d(1 - c[0], c[1], c[2], c[3]));
			l->setCoord(tSheet->id, newCoord);
		}
	}

	// Rotate if need
	Scalar cosAngle = dot(sU.normalized(), tU.normalized());
	Scalar cos45 = sqrtf(2.0) / 2;

	// Do Nothing
	if (cosAngle > cos45)
	{
		tCtrlPointNew = tCtrlPoint;
		tCtrlWeightNew = tCtrlWeight;
	}
	// Rotate 180 degrees
	else if (cosAngle < -cos45)
	{
		//  --> sV				tU
		// |					|
		// sU             tV <--
		// By flipping along both directions
		std::reverse(tCtrlPoint.begin(), tCtrlPoint.end());
		std::reverse(tCtrlWeight.begin(), tCtrlWeight.end());

		for (int i = 0; i < (int)tCtrlPoint.size(); i++)
		{
			std::reverse(tCtrlPoint[i].begin(), tCtrlPoint[i].end());
			std::reverse(tCtrlWeight[i].begin(), tCtrlWeight[i].end());
		}

		// The new control points and weights
		tCtrlPointNew = tCtrlPoint;
		tCtrlWeightNew = tCtrlWeight;
		isModified = true;

		// Update the coordinates of links
		foreach(Structure::Link * l, tgt->getEdges(tSheet->id)){
			Array1D_Vector4d oldCoord = l->getCoord(tSheet->id), newCoord;
			foreach(Vector4d c, oldCoord) newCoord.push_back(Vector4d(1 - c[0], 1 - c[1], c[2], c[3]));
			l->setCoord(tSheet->id, newCoord);
		}
	}
	// Rotate 90 degrees 
	else
	{
		Vector3 stU = cross(sU, tU);
		if (dot(stU, sUV) >= 0)
		{
			//  --> sV		tV
			// |			|
			// sU           --> tU
			// Transpose and reverse along U
			tCtrlPointNew = transpose<Vector3>(tCtrlPoint);
			tCtrlWeightNew = transpose<Scalar>(tCtrlWeight);

			std::reverse(tCtrlPointNew.begin(), tCtrlPointNew.end());
			std::reverse(tCtrlWeightNew.begin(), tCtrlWeightNew.end());

			// Update the coordinates of links
			foreach(Structure::Link * l, tgt->getEdges(tSheet->id)){
				Array1D_Vector4d oldCoord = l->getCoord(tSheet->id), newCoord;
				foreach(Vector4d c, oldCoord) newCoord.push_back(Vector4d(1 - c[1], c[0], c[2], c[3]));
				l->setCoord(tSheet->id, newCoord);
			}
		}
		else
		{
			//  --> sV	tU<--
			// |			 |
			// sU			tV
			// Reverse along U and Transpose
			std::reverse(tCtrlPoint.begin(), tCtrlPoint.end());
			std::reverse(tCtrlWeight.begin(), tCtrlWeight.end());

			tCtrlPointNew = transpose<Vector3>(tCtrlPoint);
			tCtrlWeightNew = transpose<Scalar>(tCtrlWeight);

			// Update the coordinates of links
			foreach(Structure::Link * l, tgt->getEdges(tSheet->id)){
				Array1D_Vector4d oldCoord = l->getCoord(tSheet->id), newCoord;
				foreach(Vector4d c, oldCoord) newCoord.push_back(Vector4d(c[1], 1 - c[0], c[2], c[3]));
				l->setCoord(tSheet->id, newCoord);
			}
		}

		isModified = true;
		isUVFlipped = true;
	}

	// Create a new sheet if need
	if (isModified)
	{
		NURBS::NURBSRectangled newRect;
		if (isUVFlipped)
			newRect = NURBS::NURBSRectangled(tCtrlPointNew, tCtrlWeightNew, vDegree, uDegree, vLoop, uLoop, vOpen, uOpen);
		else
			newRect = NURBS::NURBSRectangled(tCtrlPointNew, tCtrlWeightNew, uDegree, vDegree, uLoop, vLoop, uOpen, vOpen);

		tSheet->surface = newRect;
	}
}

void NURBSTools::modifyGraph()
{
	GraphModifyWidget * widget = new GraphModifyWidget(plugin->graph);
	plugin->connect(widget, SIGNAL(updateView()), SLOT(updateDrawArea()));
	widget->show();
}

void NURBSTools::convertToCurve()
{
	QStringList selected = selectedGroups();
	ui->partsList->clearSelection();

	if( selected.isEmpty() ){
		plugin->convertToCurve();
		plugin->updateDrawArea();
		fillList();
		return;
	}

	foreach(QString gid, selected){
		plugin->selectGroup(gid);
		plugin->convertToCurve();
	}

	if(selected.size() < 2) return;

	/// Post process:
    QColor groupColor = starlab::qRandomColor2();
	Structure::Node * firstElement = plugin->graph->getNode(selected.front());

	foreach(QString gid, selected)
	{
		// Change color
		Structure::Node * n = plugin->graph->getNode(gid);
		n->vis_property["color"].setValue( groupColor );

		// Align to first in group
		correspondTwoCurves((Structure::Curve*)firstElement, (Structure::Curve*)n, plugin->graph);
	}

	plugin->graph->addGroup(selected.toVector());

	plugin->updateDrawArea();
	fillList();
}

void NURBSTools::convertToSheet()
{
	QStringList selected = selectedGroups();
	ui->partsList->clearSelection();

	if( selected.isEmpty() ){
		plugin->convertToSheet();
		plugin->updateDrawArea();
		fillList();
		return;
	}

	foreach(QString gid, selected){
		plugin->selectGroup(gid);
		plugin->convertToSheet();
	}

	if(selected.size() < 2) return;

	/// Post process:
    QColor groupColor = starlab::qRandomColor2();
	Structure::Node * firstElement = plugin->graph->getNode(selected.front());

	foreach(QString gid, selected)
	{
		// Change color
		Structure::Node * n = plugin->graph->getNode(gid);
		n->vis_property["color"].setValue( groupColor );

		// Align to first in group
		correspondTwoSheets((Structure::Sheet*)firstElement, (Structure::Sheet*)n, plugin->graph);
	}

	plugin->graph->addGroup(selected.toVector());

	plugin->updateDrawArea();
	fillList();
}
