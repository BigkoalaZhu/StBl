/********************************************************************************
** Form generated from reading UI file 'GraphModifyWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHMODIFYWIDGET_H
#define UI_GRAPHMODIFYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_GraphModifyWidget
{
public:
    QGridLayout *gridLayout;
    QListWidget *list2;
    QPushButton *renameButton;
    QListWidget *list1;
    QPushButton *unlinkButton;
    QSpacerItem *verticalSpacer;
    QPushButton *linkButton;
    QSpacerItem *verticalSpacer_2;
    QPushButton *unlinkAllButton;
    QPushButton *updateLinkButton;
    QDoubleSpinBox *uA;
    QDoubleSpinBox *vA;
    QDoubleSpinBox *vB;
    QPushButton *removeButton;
    QDoubleSpinBox *uB;

    void setupUi(QDialog *GraphModifyWidget)
    {
        if (GraphModifyWidget->objectName().isEmpty())
            GraphModifyWidget->setObjectName(QStringLiteral("GraphModifyWidget"));
        GraphModifyWidget->setWindowModality(Qt::WindowModal);
        GraphModifyWidget->resize(518, 299);
        GraphModifyWidget->setModal(true);
        gridLayout = new QGridLayout(GraphModifyWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        list2 = new QListWidget(GraphModifyWidget);
        list2->setObjectName(QStringLiteral("list2"));
        list2->setAlternatingRowColors(true);
        list2->setSelectionMode(QAbstractItemView::MultiSelection);

        gridLayout->addWidget(list2, 0, 3, 5, 2);

        renameButton = new QPushButton(GraphModifyWidget);
        renameButton->setObjectName(QStringLiteral("renameButton"));

        gridLayout->addWidget(renameButton, 4, 2, 1, 1);

        list1 = new QListWidget(GraphModifyWidget);
        list1->setObjectName(QStringLiteral("list1"));
        list1->setAlternatingRowColors(true);
        list1->setSelectionMode(QAbstractItemView::SingleSelection);

        gridLayout->addWidget(list1, 0, 0, 5, 2);

        unlinkButton = new QPushButton(GraphModifyWidget);
        unlinkButton->setObjectName(QStringLiteral("unlinkButton"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
        unlinkButton->setIcon(icon);

        gridLayout->addWidget(unlinkButton, 2, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 2, 1, 1);

        linkButton = new QPushButton(GraphModifyWidget);
        linkButton->setObjectName(QStringLiteral("linkButton"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/connect.png"), QSize(), QIcon::Normal, QIcon::Off);
        linkButton->setIcon(icon1);

        gridLayout->addWidget(linkButton, 0, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_2, 1, 2, 1, 1);

        unlinkAllButton = new QPushButton(GraphModifyWidget);
        unlinkAllButton->setObjectName(QStringLiteral("unlinkAllButton"));

        gridLayout->addWidget(unlinkAllButton, 5, 2, 1, 1);

        updateLinkButton = new QPushButton(GraphModifyWidget);
        updateLinkButton->setObjectName(QStringLiteral("updateLinkButton"));

        gridLayout->addWidget(updateLinkButton, 6, 2, 1, 1);

        uA = new QDoubleSpinBox(GraphModifyWidget);
        uA->setObjectName(QStringLiteral("uA"));
        uA->setDecimals(6);
        uA->setMaximum(1);
        uA->setSingleStep(0.1);

        gridLayout->addWidget(uA, 6, 0, 1, 1);

        vA = new QDoubleSpinBox(GraphModifyWidget);
        vA->setObjectName(QStringLiteral("vA"));
        vA->setDecimals(6);
        vA->setMaximum(1);
        vA->setSingleStep(0.1);

        gridLayout->addWidget(vA, 6, 1, 1, 1);

        vB = new QDoubleSpinBox(GraphModifyWidget);
        vB->setObjectName(QStringLiteral("vB"));
        vB->setDecimals(6);
        vB->setMaximum(1);

        gridLayout->addWidget(vB, 6, 4, 1, 1);

        removeButton = new QPushButton(GraphModifyWidget);
        removeButton->setObjectName(QStringLiteral("removeButton"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/minus.png"), QSize(), QIcon::Normal, QIcon::Off);
        removeButton->setIcon(icon2);

        gridLayout->addWidget(removeButton, 5, 3, 1, 1);

        uB = new QDoubleSpinBox(GraphModifyWidget);
        uB->setObjectName(QStringLiteral("uB"));
        uB->setDecimals(6);
        uB->setMaximum(1);
        uB->setSingleStep(0.1);

        gridLayout->addWidget(uB, 6, 3, 1, 1);


        retranslateUi(GraphModifyWidget);

        QMetaObject::connectSlotsByName(GraphModifyWidget);
    } // setupUi

    void retranslateUi(QDialog *GraphModifyWidget)
    {
        GraphModifyWidget->setWindowTitle(QApplication::translate("GraphModifyWidget", "Modify Graph", 0));
        renameButton->setText(QApplication::translate("GraphModifyWidget", "Rename..", 0));
        unlinkButton->setText(QApplication::translate("GraphModifyWidget", "Unlink", 0));
        linkButton->setText(QApplication::translate("GraphModifyWidget", "Link", 0));
        unlinkAllButton->setText(QApplication::translate("GraphModifyWidget", "Unlink all", 0));
        updateLinkButton->setText(QApplication::translate("GraphModifyWidget", "Update", 0));
        removeButton->setText(QApplication::translate("GraphModifyWidget", "Remove node", 0));
    } // retranslateUi

};

namespace Ui {
    class GraphModifyWidget: public Ui_GraphModifyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHMODIFYWIDGET_H
