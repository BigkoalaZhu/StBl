#pragma once
#include "ui_stbl_widget.h"
#include "SurfaceMeshPlugins.h"

class structureblending_mode;
namespace Ui { class stbl_widget; }

class stbl_widget: public QWidget{
    Q_OBJECT
public:
    explicit stbl_widget(structureblending_mode * m = 0);
    ~stbl_widget();

	Ui::stbl_widget *ui;
private:
    structureblending_mode *mode;

public slots:

};
