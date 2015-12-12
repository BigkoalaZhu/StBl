/********************************************************************************
** Form generated from reading UI file 'GraphExplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHEXPLORER_H
#define UI_GRAPHEXPLORER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphExplorer
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *graphNameLayout;
    QLabel *graphName;
    QVBoxLayout *graphVizLayout;
    QWidget *graphVizWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *graphImage;
    QGridLayout *gridLayout;
    QLineEdit *nodesFilterProperty;
    QLabel *label_2;
    QLabel *label_5;
    QLineEdit *nodesFilterValue;
    QLabel *label_3;
    QTreeWidget *nodesTree;
    QCheckBox *isGeometry;
    QCheckBox *isActual;
    QGridLayout *gridLayout_2;
    QTreeWidget *edgesTree;
    QLabel *label_7;
    QLabel *label;
    QTreeWidget *graphTree;
    QGridLayout *gridLayout_3;
    QLineEdit *edgesFilterValue;
    QLabel *label_6;
    QLabel *label_4;
    QLineEdit *edgesFilterProperty;

    void setupUi(QWidget *GraphExplorer)
    {
        if (GraphExplorer->objectName().isEmpty())
            GraphExplorer->setObjectName(QStringLiteral("GraphExplorer"));
        GraphExplorer->resize(553, 918);
        verticalLayout = new QVBoxLayout(GraphExplorer);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        graphNameLayout = new QVBoxLayout();
        graphNameLayout->setObjectName(QStringLiteral("graphNameLayout"));
        graphName = new QLabel(GraphExplorer);
        graphName->setObjectName(QStringLiteral("graphName"));

        graphNameLayout->addWidget(graphName);


        verticalLayout->addLayout(graphNameLayout);

        graphVizLayout = new QVBoxLayout();
        graphVizLayout->setObjectName(QStringLiteral("graphVizLayout"));
        graphVizWidget = new QWidget(GraphExplorer);
        graphVizWidget->setObjectName(QStringLiteral("graphVizWidget"));
        verticalLayout_2 = new QVBoxLayout(graphVizWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        graphImage = new QLabel(graphVizWidget);
        graphImage->setObjectName(QStringLiteral("graphImage"));

        verticalLayout_2->addWidget(graphImage);


        graphVizLayout->addWidget(graphVizWidget);


        verticalLayout->addLayout(graphVizLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        nodesFilterProperty = new QLineEdit(GraphExplorer);
        nodesFilterProperty->setObjectName(QStringLiteral("nodesFilterProperty"));

        gridLayout->addWidget(nodesFilterProperty, 2, 1, 1, 1);

        label_2 = new QLabel(GraphExplorer);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label_5 = new QLabel(GraphExplorer);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 2, 2, 1, 1);

        nodesFilterValue = new QLineEdit(GraphExplorer);
        nodesFilterValue->setObjectName(QStringLiteral("nodesFilterValue"));

        gridLayout->addWidget(nodesFilterValue, 2, 3, 1, 1);

        label_3 = new QLabel(GraphExplorer);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        nodesTree = new QTreeWidget(GraphExplorer);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(1, QStringLiteral("2"));
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        nodesTree->setHeaderItem(__qtreewidgetitem);
        nodesTree->setObjectName(QStringLiteral("nodesTree"));
        nodesTree->setSortingEnabled(true);
        nodesTree->setColumnCount(2);

        gridLayout->addWidget(nodesTree, 1, 0, 1, 4);

        isGeometry = new QCheckBox(GraphExplorer);
        isGeometry->setObjectName(QStringLiteral("isGeometry"));

        gridLayout->addWidget(isGeometry, 3, 3, 1, 1);

        isActual = new QCheckBox(GraphExplorer);
        isActual->setObjectName(QStringLiteral("isActual"));

        gridLayout->addWidget(isActual, 3, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        edgesTree = new QTreeWidget(GraphExplorer);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(1, QStringLiteral("2"));
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        edgesTree->setHeaderItem(__qtreewidgetitem1);
        edgesTree->setObjectName(QStringLiteral("edgesTree"));
        edgesTree->setSortingEnabled(true);
        edgesTree->setWordWrap(true);
        edgesTree->setColumnCount(2);

        gridLayout_2->addWidget(edgesTree, 1, 0, 1, 4);

        label_7 = new QLabel(GraphExplorer);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_2->addWidget(label_7, 0, 4, 1, 1);

        label = new QLabel(GraphExplorer);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        graphTree = new QTreeWidget(GraphExplorer);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(1, QStringLiteral("2"));
        __qtreewidgetitem2->setText(0, QStringLiteral("1"));
        graphTree->setHeaderItem(__qtreewidgetitem2);
        graphTree->setObjectName(QStringLiteral("graphTree"));
        graphTree->setEditTriggers(QAbstractItemView::AllEditTriggers);
        graphTree->setSortingEnabled(true);
        graphTree->setWordWrap(true);
        graphTree->setColumnCount(2);

        gridLayout_2->addWidget(graphTree, 1, 4, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        edgesFilterValue = new QLineEdit(GraphExplorer);
        edgesFilterValue->setObjectName(QStringLiteral("edgesFilterValue"));

        gridLayout_3->addWidget(edgesFilterValue, 0, 3, 1, 1);

        label_6 = new QLabel(GraphExplorer);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 0, 2, 1, 1);

        label_4 = new QLabel(GraphExplorer);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_3->addWidget(label_4, 0, 0, 1, 1);

        edgesFilterProperty = new QLineEdit(GraphExplorer);
        edgesFilterProperty->setObjectName(QStringLiteral("edgesFilterProperty"));

        gridLayout_3->addWidget(edgesFilterProperty, 0, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout_3, 3, 0, 1, 5);


        verticalLayout->addLayout(gridLayout_2);


        retranslateUi(GraphExplorer);

        QMetaObject::connectSlotsByName(GraphExplorer);
    } // setupUi

    void retranslateUi(QWidget *GraphExplorer)
    {
        GraphExplorer->setWindowTitle(QApplication::translate("GraphExplorer", "Graph Explorer", 0));
        graphName->setText(QApplication::translate("GraphExplorer", "Graph name", 0));
        graphImage->setText(QApplication::translate("GraphExplorer", "Graph Image", 0));
        label_2->setText(QApplication::translate("GraphExplorer", "Nodes", 0));
        label_5->setText(QApplication::translate("GraphExplorer", "Value", 0));
        label_3->setText(QApplication::translate("GraphExplorer", "Property", 0));
        isGeometry->setText(QApplication::translate("GraphExplorer", "Geometry", 0));
        isActual->setText(QApplication::translate("GraphExplorer", "Actual Graph", 0));
        label_7->setText(QApplication::translate("GraphExplorer", "Graph", 0));
        label->setText(QApplication::translate("GraphExplorer", "Edges", 0));
        label_6->setText(QApplication::translate("GraphExplorer", "Value", 0));
        label_4->setText(QApplication::translate("GraphExplorer", "Property", 0));
    } // retranslateUi

};

namespace Ui {
    class GraphExplorer: public Ui_GraphExplorer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHEXPLORER_H
