#pragma once
#include "StarlabDrawArea.h"
#include "SurfaceMeshPlugins.h"
#include "SurfaceMeshHelper.h"
#include "GenerateProjectedImage.h"
#include "CorrFinder.h"

#include "stbl_widget.h"

class structureblending_mode : public SurfaceMeshModePlugin{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "structureblending_mode.plugin.starlab")
    Q_INTERFACES(ModePlugin)

public:
    structureblending_mode();

    QIcon icon(){ return QIcon(":/structureblending_mode.png"); }

    /// Functions part of the EditPlugin system
    void create();
    void destroy(){}

    void decorate();
    void drawWithNames();

	void update();

    /// User interactions
    bool wheelEvent(QWheelEvent *);
    bool mouseMoveEvent(QMouseEvent*);
    bool keyPressEvent(QKeyEvent *);

	bool isApplicable() { return true; }

private:
    stbl_widget * widget;
	
	SurfaceMeshModel * Current_Single_Object;
	std::vector<SurfaceMeshModel*> Current_parts;

	Vector3VertexProperty points, src_points;
	Vector3FaceProperty fnormals;

	QString CameraPath;
	GenerateProjectedImage * projectImage;
	int CameraIndex;

	/////////////////////////////////////////////Correspondence
	CorrFinder * corrfinder;
	bool hasPart;
	bool hasInbetween;

public slots:
    void LoadSingleObject();
	void LoadSingleObjectSplit();
	void LoadListSplit();
	void Filtering();

	void CameraPathChange(QString);
	void LoadSingleMesh();
	void CameraIndexChange(QString);
	void GenerateSingleImage();
	void LoadAList2GenerateImages();

	void HasPartChange(int);
	void HasInbetweenChange(int);

	/////////////////////////////////////////////Correspondence
	void LoadShapePair();
	void GeneratePartSet();
	void SourceSeleclPart(QModelIndex);
	void TargetSeleclPart(QModelIndex);
	
};


