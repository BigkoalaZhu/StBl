#include "stbl_widget.h"
#include "ui_stbl_widget.h"
#include "structureblending_mode.h"

stbl_widget::stbl_widget(structureblending_mode * m) : ui(new Ui::stbl_widget)
{
    ui->setupUi(this);

    this->mode = m;

	mode->connect(ui->LoadSingle, SIGNAL(clicked()), SLOT(LoadSingleObject()));
	mode->connect(ui->SplitSingle, SIGNAL(clicked()), SLOT(LoadSingleObjectSplit()));
	mode->connect(ui->SplitList, SIGNAL(clicked()), SLOT(LoadListSplit()));
	mode->connect(ui->Filtering, SIGNAL(clicked()), SLOT(Filtering()));

	mode->connect(ui->GenSingleImage, SIGNAL(clicked()), SLOT(GenerateSingleImage()));
	mode->connect(ui->LoadSigleMesh, SIGNAL(clicked()), SLOT(LoadSingleMesh()));
	mode->connect(ui->cameraIndex, SIGNAL(textChanged(QString)), SLOT(CameraIndexChange(QString)));
	mode->connect(ui->cameraPath, SIGNAL(textChanged(QString)), SLOT(CameraPathChange(QString)));
	mode->connect(ui->LoadAList, SIGNAL(clicked()), SLOT(LoadAList2GenerateImages()));

	/////////////////////////////////////////////Correspondence
	mode->connect(ui->HasPartInfo, SIGNAL(stateChanged(int)), SLOT(HasPartChange(int)));
	mode->connect(ui->HasInbetInfo, SIGNAL(stateChanged(int)), SLOT(HasInbetweenChange(int)));
	mode->connect(ui->LoadPair, SIGNAL(clicked()), SLOT(LoadShapePair()));
	
}

stbl_widget::~stbl_widget()
{
    delete ui;
}
