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
}

stbl_widget::~stbl_widget()
{
    delete ui;
}
