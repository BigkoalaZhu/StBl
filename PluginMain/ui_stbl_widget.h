/********************************************************************************
** Form generated from reading UI file 'stbl_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STBL_WIDGET_H
#define UI_STBL_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_stbl_widget
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *Obj2Graph;
    QFormLayout *formLayout_2;
    QLabel *label;
    QLineEdit *VoxelResolution;
    QPushButton *LoadSingle;
    QWidget *SS;
    QFormLayout *formLayout_3;
    QPushButton *SplitSingle;
    QPushButton *SplitList;

    void setupUi(QWidget *stbl_widget)
    {
        if (stbl_widget->objectName().isEmpty())
            stbl_widget->setObjectName(QStringLiteral("stbl_widget"));
        stbl_widget->resize(349, 545);
        verticalLayout = new QVBoxLayout(stbl_widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(stbl_widget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        Obj2Graph = new QWidget();
        Obj2Graph->setObjectName(QStringLiteral("Obj2Graph"));
        formLayout_2 = new QFormLayout(Obj2Graph);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(Obj2Graph);
        label->setObjectName(QStringLiteral("label"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label);

        VoxelResolution = new QLineEdit(Obj2Graph);
        VoxelResolution->setObjectName(QStringLiteral("VoxelResolution"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, VoxelResolution);

        LoadSingle = new QPushButton(Obj2Graph);
        LoadSingle->setObjectName(QStringLiteral("LoadSingle"));

        formLayout_2->setWidget(0, QFormLayout::SpanningRole, LoadSingle);

        tabWidget->addTab(Obj2Graph, QString());
        SS = new QWidget();
        SS->setObjectName(QStringLiteral("SS"));
        formLayout_3 = new QFormLayout(SS);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        formLayout_3->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        SplitSingle = new QPushButton(SS);
        SplitSingle->setObjectName(QStringLiteral("SplitSingle"));

        formLayout_3->setWidget(0, QFormLayout::SpanningRole, SplitSingle);

        SplitList = new QPushButton(SS);
        SplitList->setObjectName(QStringLiteral("SplitList"));

        formLayout_3->setWidget(1, QFormLayout::SpanningRole, SplitList);

        tabWidget->addTab(SS, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(stbl_widget);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(stbl_widget);
    } // setupUi

    void retranslateUi(QWidget *stbl_widget)
    {
        stbl_widget->setWindowTitle(QApplication::translate("stbl_widget", "Structure Blending", 0));
        label->setText(QApplication::translate("stbl_widget", "Voxel resolution:", 0));
        LoadSingle->setText(QApplication::translate("stbl_widget", "Load Single Segmented Object", 0));
        tabWidget->setTabText(tabWidget->indexOf(Obj2Graph), QApplication::translate("stbl_widget", "Object To Graph", 0));
        SplitSingle->setText(QApplication::translate("stbl_widget", "Load single shape and split", 0));
        SplitList->setText(QApplication::translate("stbl_widget", "Load a List to split", 0));
        tabWidget->setTabText(tabWidget->indexOf(SS), QApplication::translate("stbl_widget", "Split Shapes", 0));
    } // retranslateUi

};

namespace Ui {
    class stbl_widget: public Ui_stbl_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STBL_WIDGET_H
