/********************************************************************************
** Form generated from reading UI file 'SchedulerWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCHEDULERWIDGET_H
#define UI_SCHEDULERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SchedulerWidget
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *mainWidget;
    QGridLayout *gridLayout;
    QWidget *nodesWrapperWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *nodesLabel;
    QWidget *nodesWidget;
    QVBoxLayout *verticalLayout_4;
    QListWidget *nodesList;
    QWidget *blendControls;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *stopButton;
    QPushButton *diffTimeButton;
    QDoubleSpinBox *gdResolution;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QLabel *label_3;
    QPushButton *defaultScheduleButton;
    QDoubleSpinBox *timeStep;
    QLabel *label_4;
    QPushButton *analyzeButton;
    QWidget *tasksWrapperWidget;
    QVBoxLayout *verticalLayout_3;
    QLabel *timelineLabel;
    QGraphicsView *timelineView;
    QWidget *tasksWidget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *showHideLayout;
    QWidget *widget;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label_10;
    QPushButton *showAllButton;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *blendButton;
    QPushButton *sameTimeButton;
    QSpacerItem *horizontalSpacer_7;
    QProgressBar *progressBar;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *shuffleButton;
    QGroupBox *synthesisControl;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QSpinBox *reconLevel;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *draftRenderButton;
    QPushButton *renderCurrentButton;
    QSpacerItem *horizontalSpacer_4;
    QSpinBox *renderCount;
    QPushButton *cleanUpButton;
    QPushButton *renderAllButton;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_5;
    QSpinBox *startPercentage;

    void setupUi(QWidget *SchedulerWidget)
    {
        if (SchedulerWidget->objectName().isEmpty())
            SchedulerWidget->setObjectName(QStringLiteral("SchedulerWidget"));
        SchedulerWidget->resize(918, 564);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SchedulerWidget->sizePolicy().hasHeightForWidth());
        SchedulerWidget->setSizePolicy(sizePolicy);
        SchedulerWidget->setMinimumSize(QSize(0, 300));
        verticalLayout = new QVBoxLayout(SchedulerWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainWidget = new QWidget(SchedulerWidget);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        gridLayout = new QGridLayout(mainWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        nodesWrapperWidget = new QWidget(mainWidget);
        nodesWrapperWidget->setObjectName(QStringLiteral("nodesWrapperWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(nodesWrapperWidget->sizePolicy().hasHeightForWidth());
        nodesWrapperWidget->setSizePolicy(sizePolicy1);
        nodesWrapperWidget->setMaximumSize(QSize(150, 16777215));
        nodesWrapperWidget->setStyleSheet(QStringLiteral("background:#363636"));
        verticalLayout_2 = new QVBoxLayout(nodesWrapperWidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        nodesLabel = new QLabel(nodesWrapperWidget);
        nodesLabel->setObjectName(QStringLiteral("nodesLabel"));
        nodesLabel->setStyleSheet(QStringLiteral("background:#404040; color: #BCBCBC;"));
        nodesLabel->setAlignment(Qt::AlignCenter);
        nodesLabel->setMargin(5);

        verticalLayout_2->addWidget(nodesLabel);

        nodesWidget = new QWidget(nodesWrapperWidget);
        nodesWidget->setObjectName(QStringLiteral("nodesWidget"));
        verticalLayout_4 = new QVBoxLayout(nodesWidget);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        nodesList = new QListWidget(nodesWidget);
        nodesList->setObjectName(QStringLiteral("nodesList"));
        nodesList->setStyleSheet(QStringLiteral("color: #BCBCBC;"));
        nodesList->setFrameShape(QFrame::NoFrame);

        verticalLayout_4->addWidget(nodesList);


        verticalLayout_2->addWidget(nodesWidget);


        gridLayout->addWidget(nodesWrapperWidget, 3, 0, 1, 1);

        blendControls = new QWidget(mainWidget);
        blendControls->setObjectName(QStringLiteral("blendControls"));
        blendControls->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(blendControls->sizePolicy().hasHeightForWidth());
        blendControls->setSizePolicy(sizePolicy2);
        gridLayout_2 = new QGridLayout(blendControls);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 13, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 0, 10, 1, 1);

        stopButton = new QPushButton(blendControls);
        stopButton->setObjectName(QStringLiteral("stopButton"));

        gridLayout_2->addWidget(stopButton, 0, 11, 1, 1);

        diffTimeButton = new QPushButton(blendControls);
        diffTimeButton->setObjectName(QStringLiteral("diffTimeButton"));

        gridLayout_2->addWidget(diffTimeButton, 0, 7, 1, 1);

        gdResolution = new QDoubleSpinBox(blendControls);
        gdResolution->setObjectName(QStringLiteral("gdResolution"));
        gdResolution->setDecimals(5);
        gdResolution->setMinimum(1e-05);
        gdResolution->setMaximum(0.1);
        gdResolution->setSingleStep(0.001);
        gdResolution->setValue(0.015);

        gridLayout_2->addWidget(gdResolution, 2, 1, 1, 1);

        saveButton = new QPushButton(blendControls);
        saveButton->setObjectName(QStringLiteral("saveButton"));

        gridLayout_2->addWidget(saveButton, 0, 1, 1, 1);

        loadButton = new QPushButton(blendControls);
        loadButton->setObjectName(QStringLiteral("loadButton"));

        gridLayout_2->addWidget(loadButton, 0, 0, 1, 1);

        label_3 = new QLabel(blendControls);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        defaultScheduleButton = new QPushButton(blendControls);
        defaultScheduleButton->setObjectName(QStringLiteral("defaultScheduleButton"));

        gridLayout_2->addWidget(defaultScheduleButton, 0, 3, 1, 1);

        timeStep = new QDoubleSpinBox(blendControls);
        timeStep->setObjectName(QStringLiteral("timeStep"));
        timeStep->setDecimals(4);
        timeStep->setMinimum(0.0001);
        timeStep->setMaximum(0.1);
        timeStep->setSingleStep(0.005);
        timeStep->setValue(0.01);

        gridLayout_2->addWidget(timeStep, 2, 3, 1, 1);

        label_4 = new QLabel(blendControls);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 2, 2, 1, 1);

        analyzeButton = new QPushButton(blendControls);
        analyzeButton->setObjectName(QStringLiteral("analyzeButton"));
        analyzeButton->setEnabled(false);

        gridLayout_2->addWidget(analyzeButton, 0, 9, 1, 1);


        gridLayout->addWidget(blendControls, 0, 0, 1, 2);

        tasksWrapperWidget = new QWidget(mainWidget);
        tasksWrapperWidget->setObjectName(QStringLiteral("tasksWrapperWidget"));
        tasksWrapperWidget->setStyleSheet(QStringLiteral("background:#363636"));
        verticalLayout_3 = new QVBoxLayout(tasksWrapperWidget);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        timelineLabel = new QLabel(tasksWrapperWidget);
        timelineLabel->setObjectName(QStringLiteral("timelineLabel"));
        timelineLabel->setStyleSheet(QStringLiteral("background:#404040; color: #BCBCBC;"));
        timelineLabel->setAlignment(Qt::AlignCenter);
        timelineLabel->setMargin(5);

        verticalLayout_3->addWidget(timelineLabel);

        timelineView = new QGraphicsView(tasksWrapperWidget);
        timelineView->setObjectName(QStringLiteral("timelineView"));
        timelineView->setMouseTracking(true);
        timelineView->setStyleSheet(QLatin1String("QScrollBar:horizontal {\n"
"     border: 0px solid black;\n"
"     background: #4D4D4D;\n"
"     height: 18px;\n"
"     margin: 0px 20px 0 20px;\n"
" }\n"
" QScrollBar::handle:horizontal {\n"
"     background: #4D4D4D;\n"
"     min-width: 15px;\n"
" }\n"
" QScrollBar::add-line:horizontal {\n"
"     border: 0px solid black;\n"
"     background: #4D4D4D;\n"
"     width: 19px;\n"
"     subcontrol-position: right;\n"
"     subcontrol-origin: margin;\n"
"	image: url(:/images/rightArrow.png);\n"
" }\n"
"\n"
" QScrollBar::sub-line:horizontal {\n"
"     border: 0px solid black;\n"
"     background: #4D4D4D;\n"
"     width:19px;\n"
"     subcontrol-position: left;\n"
"     subcontrol-origin: margin;\n"
"	image: url(:/images/leftArrow.png);\n"
" }\n"
""));
        timelineView->setFrameShape(QFrame::NoFrame);
        timelineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        timelineView->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        timelineView->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);
        timelineView->setDragMode(QGraphicsView::RubberBandDrag);
        timelineView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        verticalLayout_3->addWidget(timelineView);

        tasksWidget = new QWidget(tasksWrapperWidget);
        tasksWidget->setObjectName(QStringLiteral("tasksWidget"));
        horizontalLayout_2 = new QHBoxLayout(tasksWidget);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));

        verticalLayout_3->addWidget(tasksWidget);


        gridLayout->addWidget(tasksWrapperWidget, 3, 1, 1, 1);

        showHideLayout = new QHBoxLayout();
        showHideLayout->setObjectName(QStringLiteral("showHideLayout"));
        widget = new QWidget(mainWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setStyleSheet(QStringLiteral("background:#363636;color: #BCBCBC;"));
        gridLayout_4 = new QGridLayout(widget);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_10 = new QLabel(widget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setTextFormat(Qt::RichText);

        horizontalLayout->addWidget(label_10);


        gridLayout_4->addLayout(horizontalLayout, 0, 0, 1, 1);

        showAllButton = new QPushButton(widget);
        showAllButton->setObjectName(QStringLiteral("showAllButton"));
        showAllButton->setStyleSheet(QStringLiteral(""));

        gridLayout_4->addWidget(showAllButton, 0, 6, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_6, 0, 1, 1, 1);

        blendButton = new QPushButton(widget);
        blendButton->setObjectName(QStringLiteral("blendButton"));

        gridLayout_4->addWidget(blendButton, 0, 9, 1, 1);

        sameTimeButton = new QPushButton(widget);
        sameTimeButton->setObjectName(QStringLiteral("sameTimeButton"));

        gridLayout_4->addWidget(sameTimeButton, 0, 5, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(80, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_7, 0, 8, 1, 1);

        progressBar = new QProgressBar(widget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        gridLayout_4->addWidget(progressBar, 0, 2, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_8, 0, 3, 1, 1);

        shuffleButton = new QPushButton(widget);
        shuffleButton->setObjectName(QStringLiteral("shuffleButton"));

        gridLayout_4->addWidget(shuffleButton, 0, 4, 1, 1);


        showHideLayout->addWidget(widget);


        gridLayout->addLayout(showHideLayout, 5, 0, 1, 2);

        synthesisControl = new QGroupBox(mainWidget);
        synthesisControl->setObjectName(QStringLiteral("synthesisControl"));
        gridLayout_3 = new QGridLayout(synthesisControl);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_3, 0, 13, 1, 1);

        reconLevel = new QSpinBox(synthesisControl);
        reconLevel->setObjectName(QStringLiteral("reconLevel"));
        reconLevel->setMinimum(4);
        reconLevel->setMaximum(100);
        reconLevel->setValue(7);

        gridLayout_3->addWidget(reconLevel, 0, 6, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_5, 0, 2, 1, 1);

        draftRenderButton = new QPushButton(synthesisControl);
        draftRenderButton->setObjectName(QStringLiteral("draftRenderButton"));

        gridLayout_3->addWidget(draftRenderButton, 0, 1, 1, 1);

        renderCurrentButton = new QPushButton(synthesisControl);
        renderCurrentButton->setObjectName(QStringLiteral("renderCurrentButton"));

        gridLayout_3->addWidget(renderCurrentButton, 0, 3, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_4, 0, 4, 1, 1);

        renderCount = new QSpinBox(synthesisControl);
        renderCount->setObjectName(QStringLiteral("renderCount"));
        renderCount->setMinimum(-1);
        renderCount->setMaximum(500);
        renderCount->setSingleStep(10);
        renderCount->setValue(10);

        gridLayout_3->addWidget(renderCount, 0, 8, 1, 1);

        cleanUpButton = new QPushButton(synthesisControl);
        cleanUpButton->setObjectName(QStringLiteral("cleanUpButton"));

        gridLayout_3->addWidget(cleanUpButton, 0, 0, 1, 1);

        renderAllButton = new QPushButton(synthesisControl);
        renderAllButton->setObjectName(QStringLiteral("renderAllButton"));

        gridLayout_3->addWidget(renderAllButton, 0, 12, 1, 1);

        label_2 = new QLabel(synthesisControl);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 0, 5, 1, 1);

        label = new QLabel(synthesisControl);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 7, 1, 1);

        label_5 = new QLabel(synthesisControl);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_3->addWidget(label_5, 0, 10, 1, 1);

        startPercentage = new QSpinBox(synthesisControl);
        startPercentage->setObjectName(QStringLiteral("startPercentage"));
        startPercentage->setSingleStep(10);

        gridLayout_3->addWidget(startPercentage, 0, 11, 1, 1);


        gridLayout->addWidget(synthesisControl, 4, 0, 1, 2);


        verticalLayout->addWidget(mainWidget);


        retranslateUi(SchedulerWidget);

        QMetaObject::connectSlotsByName(SchedulerWidget);
    } // setupUi

    void retranslateUi(QWidget *SchedulerWidget)
    {
        SchedulerWidget->setWindowTitle(QApplication::translate("SchedulerWidget", "Form", 0));
        nodesLabel->setText(QApplication::translate("SchedulerWidget", "Nodes", 0));
        stopButton->setText(QApplication::translate("SchedulerWidget", "Stop", 0));
        diffTimeButton->setText(QApplication::translate("SchedulerWidget", "Spread out", 0));
        saveButton->setText(QApplication::translate("SchedulerWidget", "Save", 0));
        loadButton->setText(QApplication::translate("SchedulerWidget", "Load", 0));
        label_3->setText(QApplication::translate("SchedulerWidget", "GD Resolutoin", 0));
        defaultScheduleButton->setText(QApplication::translate("SchedulerWidget", "Default", 0));
        label_4->setText(QApplication::translate("SchedulerWidget", "Time step", 0));
        analyzeButton->setText(QApplication::translate("SchedulerWidget", "Analyze..", 0));
        timelineLabel->setText(QApplication::translate("SchedulerWidget", "Timeline", 0));
        label_10->setText(QApplication::translate("SchedulerWidget", "<html>\n"
"<head/>\n"
"<body><p>\n"
"\n"
"<span style=\" font-weight:600; color:#ff6179;\">\342\226\272Shrink   &nbsp;&nbsp;&nbsp;&nbsp;   </span>\n"
"<span style=\" font-weight:600; color:#687eff;\">\342\226\272Grow  &nbsp;&nbsp;&nbsp;&nbsp;    </span>\n"
"<span style=\" font-weight:600; color:#ffa56b;\">\342\226\272Split    &nbsp;&nbsp;&nbsp;&nbsp;  </span>\n"
"<span style=\" font-weight:600; color:#ffdb58;\">\342\226\272Merge   &nbsp;&nbsp;&nbsp;&nbsp;   </span>\n"
"<span style=\" font-weight:600; color:#6bff87;\">\342\226\272Morph    &nbsp;&nbsp;&nbsp;&nbsp;  </span>\n"
"\n"
"</body>\n"
"</html>", 0));
        showAllButton->setText(QApplication::translate("SchedulerWidget", "show / hide", 0));
        blendButton->setText(QApplication::translate("SchedulerWidget", "Blend", 0));
        sameTimeButton->setText(QApplication::translate("SchedulerWidget", "Same time", 0));
        shuffleButton->setText(QApplication::translate("SchedulerWidget", "shuffle", 0));
        synthesisControl->setTitle(QApplication::translate("SchedulerWidget", "Output", 0));
        draftRenderButton->setText(QApplication::translate("SchedulerWidget", "Draft Render..", 0));
        renderCurrentButton->setText(QApplication::translate("SchedulerWidget", "Render current", 0));
#ifndef QT_NO_TOOLTIP
        renderCount->setToolTip(QApplication::translate("SchedulerWidget", "Value of -1 gives entire sequence", 0));
#endif // QT_NO_TOOLTIP
        cleanUpButton->setText(QApplication::translate("SchedulerWidget", "Clean up", 0));
        renderAllButton->setText(QApplication::translate("SchedulerWidget", "Render all..", 0));
        label_2->setText(QApplication::translate("SchedulerWidget", "Level", 0));
        label->setText(QApplication::translate("SchedulerWidget", "Frames", 0));
        label_5->setText(QApplication::translate("SchedulerWidget", "Starting(%)", 0));
    } // retranslateUi

};

namespace Ui {
    class SchedulerWidget: public Ui_SchedulerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCHEDULERWIDGET_H
