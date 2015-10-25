/********************************************************************************
** Form generated from reading UI file 'nurbstools.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NURBSTOOLS_H
#define UI_NURBSTOOLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NURBSTools
{
public:
    QGridLayout *gridLayout_3;
    QSpinBox *uCount;
    QDoubleSpinBox *resolution;
    QSpinBox *vCount;
    QPushButton *clearButton;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *saveButton;
    QLabel *label;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_7;
    QPushButton *clearSelectedButton;
    QPushButton *linksButton;
    QListWidget *partsList;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *fitCurveButton;
    QPushButton *curveFitButton;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QSpinBox *contractIterations;
    QLabel *label_4;
    QPushButton *skeletonButton;
    QPushButton *skeletonButtonStep;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_5;
    QPushButton *flipVButton;
    QPushButton *flipUButton;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *fitSurfaceButton;
    QPushButton *sheetFitButton;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_6;
    QDoubleSpinBox *voxelParamter;
    QCheckBox *remeshOption;
    QCheckBox *voxelOption;
    QCheckBox *medialOption;
    QCheckBox *protectSmallOption;
    QDoubleSpinBox *remeshParamter;
    QCheckBox *concaveHullOption;
    QPushButton *loadButton;

    void setupUi(QWidget *NURBSTools)
    {
        if (NURBSTools->objectName().isEmpty())
            NURBSTools->setObjectName(QStringLiteral("NURBSTools"));
        NURBSTools->resize(200, 697);
        gridLayout_3 = new QGridLayout(NURBSTools);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        uCount = new QSpinBox(NURBSTools);
        uCount->setObjectName(QStringLiteral("uCount"));
        uCount->setMinimum(4);
        uCount->setMaximum(500);
        uCount->setValue(8);

        gridLayout_3->addWidget(uCount, 6, 1, 1, 1);

        resolution = new QDoubleSpinBox(NURBSTools);
        resolution->setObjectName(QStringLiteral("resolution"));
        resolution->setDecimals(3);
        resolution->setMinimum(0);
        resolution->setMaximum(0.9);
        resolution->setSingleStep(0.01);
        resolution->setValue(0.015);

        gridLayout_3->addWidget(resolution, 9, 1, 1, 1);

        vCount = new QSpinBox(NURBSTools);
        vCount->setObjectName(QStringLiteral("vCount"));
        vCount->setMinimum(4);
        vCount->setMaximum(500);
        vCount->setValue(8);

        gridLayout_3->addWidget(vCount, 8, 1, 1, 1);

        clearButton = new QPushButton(NURBSTools);
        clearButton->setObjectName(QStringLiteral("clearButton"));

        gridLayout_3->addWidget(clearButton, 10, 0, 1, 1);

        label_2 = new QLabel(NURBSTools);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 8, 0, 1, 1);

        label_3 = new QLabel(NURBSTools);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_3->addWidget(label_3, 9, 0, 1, 1);

        saveButton = new QPushButton(NURBSTools);
        saveButton->setObjectName(QStringLiteral("saveButton"));

        gridLayout_3->addWidget(saveButton, 10, 1, 1, 1);

        label = new QLabel(NURBSTools);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 6, 0, 1, 1);

        groupBox_6 = new QGroupBox(NURBSTools);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_7 = new QGridLayout(groupBox_6);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        clearSelectedButton = new QPushButton(groupBox_6);
        clearSelectedButton->setObjectName(QStringLiteral("clearSelectedButton"));

        gridLayout_7->addWidget(clearSelectedButton, 1, 1, 1, 1);

        linksButton = new QPushButton(groupBox_6);
        linksButton->setObjectName(QStringLiteral("linksButton"));

        gridLayout_7->addWidget(linksButton, 1, 0, 1, 1);

        partsList = new QListWidget(groupBox_6);
        partsList->setObjectName(QStringLiteral("partsList"));
        partsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        partsList->setSelectionMode(QAbstractItemView::MultiSelection);
        partsList->setSelectionRectVisible(true);

        gridLayout_7->addWidget(partsList, 0, 0, 1, 2);


        gridLayout_3->addWidget(groupBox_6, 1, 0, 1, 2);

        groupBox = new QGroupBox(NURBSTools);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 1, 1, 1);

        fitCurveButton = new QPushButton(groupBox);
        fitCurveButton->setObjectName(QStringLiteral("fitCurveButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/nurbs_curve.png"), QSize(), QIcon::Normal, QIcon::Off);
        fitCurveButton->setIcon(icon);

        gridLayout->addWidget(fitCurveButton, 1, 0, 1, 1);

        curveFitButton = new QPushButton(groupBox);
        curveFitButton->setObjectName(QStringLiteral("curveFitButton"));

        gridLayout->addWidget(curveFitButton, 1, 3, 1, 1);


        gridLayout_3->addWidget(groupBox, 2, 0, 1, 2);

        groupBox_3 = new QGroupBox(NURBSTools);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        contractIterations = new QSpinBox(groupBox_3);
        contractIterations->setObjectName(QStringLiteral("contractIterations"));
        contractIterations->setValue(3);

        gridLayout_4->addWidget(contractIterations, 0, 2, 1, 1);

        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_4->addWidget(label_4, 0, 0, 1, 1);

        skeletonButton = new QPushButton(groupBox_3);
        skeletonButton->setObjectName(QStringLiteral("skeletonButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/nurbs_plugin/images/bone.png"), QSize(), QIcon::Normal, QIcon::Off);
        skeletonButton->setIcon(icon1);

        gridLayout_4->addWidget(skeletonButton, 2, 2, 1, 1);

        skeletonButtonStep = new QPushButton(groupBox_3);
        skeletonButtonStep->setObjectName(QStringLiteral("skeletonButtonStep"));

        gridLayout_4->addWidget(skeletonButtonStep, 2, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_3, 4, 0, 1, 2);

        groupBox_4 = new QGroupBox(NURBSTools);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_5 = new QGridLayout(groupBox_4);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        flipVButton = new QPushButton(groupBox_4);
        flipVButton->setObjectName(QStringLiteral("flipVButton"));

        gridLayout_5->addWidget(flipVButton, 0, 1, 1, 1);

        flipUButton = new QPushButton(groupBox_4);
        flipUButton->setObjectName(QStringLiteral("flipUButton"));

        gridLayout_5->addWidget(flipUButton, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_4, 5, 0, 1, 2);

        groupBox_2 = new QGroupBox(NURBSTools);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 4, 1, 1, 1);

        fitSurfaceButton = new QPushButton(groupBox_2);
        fitSurfaceButton->setObjectName(QStringLiteral("fitSurfaceButton"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/nurbs_surface.png"), QSize(), QIcon::Normal, QIcon::Off);
        fitSurfaceButton->setIcon(icon2);

        gridLayout_2->addWidget(fitSurfaceButton, 4, 0, 1, 1);

        sheetFitButton = new QPushButton(groupBox_2);
        sheetFitButton->setObjectName(QStringLiteral("sheetFitButton"));

        gridLayout_2->addWidget(sheetFitButton, 4, 3, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 3, 0, 1, 2);

        groupBox_5 = new QGroupBox(NURBSTools);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_6 = new QGridLayout(groupBox_5);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        voxelParamter = new QDoubleSpinBox(groupBox_5);
        voxelParamter->setObjectName(QStringLiteral("voxelParamter"));
        voxelParamter->setDecimals(3);
        voxelParamter->setMinimum(0.001);
        voxelParamter->setMaximum(1);
        voxelParamter->setSingleStep(0.01);
        voxelParamter->setValue(0.05);

        gridLayout_6->addWidget(voxelParamter, 1, 1, 1, 1);

        remeshOption = new QCheckBox(groupBox_5);
        remeshOption->setObjectName(QStringLiteral("remeshOption"));
        remeshOption->setChecked(true);

        gridLayout_6->addWidget(remeshOption, 0, 0, 1, 1);

        voxelOption = new QCheckBox(groupBox_5);
        voxelOption->setObjectName(QStringLiteral("voxelOption"));

        gridLayout_6->addWidget(voxelOption, 1, 0, 1, 1);

        medialOption = new QCheckBox(groupBox_5);
        medialOption->setObjectName(QStringLiteral("medialOption"));

        gridLayout_6->addWidget(medialOption, 3, 0, 1, 1);

        protectSmallOption = new QCheckBox(groupBox_5);
        protectSmallOption->setObjectName(QStringLiteral("protectSmallOption"));

        gridLayout_6->addWidget(protectSmallOption, 4, 0, 1, 2);

        remeshParamter = new QDoubleSpinBox(groupBox_5);
        remeshParamter->setObjectName(QStringLiteral("remeshParamter"));
        remeshParamter->setDecimals(3);
        remeshParamter->setMaximum(10000);
        remeshParamter->setSingleStep(0.005);
        remeshParamter->setValue(0.02);

        gridLayout_6->addWidget(remeshParamter, 0, 1, 1, 1);

        concaveHullOption = new QCheckBox(groupBox_5);
        concaveHullOption->setObjectName(QStringLiteral("concaveHullOption"));

        gridLayout_6->addWidget(concaveHullOption, 2, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_5, 0, 0, 1, 2);

        loadButton = new QPushButton(NURBSTools);
        loadButton->setObjectName(QStringLiteral("loadButton"));

        gridLayout_3->addWidget(loadButton, 11, 0, 1, 1);


        retranslateUi(NURBSTools);

        QMetaObject::connectSlotsByName(NURBSTools);
    } // setupUi

    void retranslateUi(QWidget *NURBSTools)
    {
        NURBSTools->setWindowTitle(QApplication::translate("NURBSTools", "Form", 0));
        clearButton->setText(QApplication::translate("NURBSTools", "Clear All", 0));
        label_2->setText(QApplication::translate("NURBSTools", "V count", 0));
        label_3->setText(QApplication::translate("NURBSTools", "Resolution", 0));
        saveButton->setText(QApplication::translate("NURBSTools", "Save", 0));
        label->setText(QApplication::translate("NURBSTools", "U count", 0));
        groupBox_6->setTitle(QApplication::translate("NURBSTools", "Batch Operation", 0));
        clearSelectedButton->setText(QApplication::translate("NURBSTools", "Clear", 0));
        linksButton->setText(QApplication::translate("NURBSTools", "Links..", 0));
        groupBox->setTitle(QApplication::translate("NURBSTools", "Curve fitting", 0));
        fitCurveButton->setText(QApplication::translate("NURBSTools", "Fit", 0));
        curveFitButton->setText(QApplication::translate("NURBSTools", "Curve fit", 0));
        groupBox_3->setTitle(QApplication::translate("NURBSTools", "Quick skeleton", 0));
        label_4->setText(QApplication::translate("NURBSTools", "Iterations", 0));
        skeletonButton->setText(QApplication::translate("NURBSTools", "Contract", 0));
        skeletonButtonStep->setText(QApplication::translate("NURBSTools", "More..", 0));
        groupBox_4->setTitle(QApplication::translate("NURBSTools", "Tools", 0));
        flipVButton->setText(QApplication::translate("NURBSTools", "Flip V", 0));
        flipUButton->setText(QApplication::translate("NURBSTools", "Flip U", 0));
        groupBox_2->setTitle(QApplication::translate("NURBSTools", "Surface fitting", 0));
        fitSurfaceButton->setText(QApplication::translate("NURBSTools", "Fit", 0));
        sheetFitButton->setText(QApplication::translate("NURBSTools", "Sheet fit", 0));
        groupBox_5->setTitle(QApplication::translate("NURBSTools", "Options", 0));
        remeshOption->setText(QApplication::translate("NURBSTools", "Remesh", 0));
        voxelOption->setText(QApplication::translate("NURBSTools", "Voxelize", 0));
        medialOption->setText(QApplication::translate("NURBSTools", "Use medial", 0));
        protectSmallOption->setText(QApplication::translate("NURBSTools", "Protect small features", 0));
        concaveHullOption->setText(QApplication::translate("NURBSTools", "Concavehull", 0));
        loadButton->setText(QApplication::translate("NURBSTools", "Load..", 0));
    } // retranslateUi

};

namespace Ui {
    class NURBSTools: public Ui_NURBSTools {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NURBSTOOLS_H
