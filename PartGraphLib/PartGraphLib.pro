include($$[STARLAB])
include($$[SURFACEMESH])
include($$[CHOLMOD])

include($$[NANOFLANN])
include($$[OCTREE])
StarlabTemplate(none)

TEMPLATE = lib
CONFIG += staticlib
QT += opengl xml svg concurrent

# Build flag
CONFIG(debug, debug|release) {
    CFG = debug
} else {
    CFG = release

    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# Library name and destination
TARGET = PartGraphLib
DESTDIR = $$PWD/$$CFG/lib

# NURBS library
LIBS += -L$$PWD/../ExternalTools/NURBS/$$CFG/lib -lNURBS
INCLUDEPATH += ../ExternalTools/NURBS

# Surface Reconstruction library
LIBS += -L$$PWD/../ExternalTools/Reconstruction/$$CFG/lib -lReconstruction
INCLUDEPATH += ../ExternalTools/Reconstruction

# Splat Rendering library
LIBS += -L$$PWD/../ExternalTools/GlSplatRendererLib/$$CFG/lib -lGlSplatRendererLib
INCLUDEPATH += ../ExternalTools/GlSplatRendererLib

HEADERS += PartNode.h \
    PartGraph.h \
    PartCurve.h \
    PartSheet.h \
    PartLink.h \
	PartGlobal.h \
	GraphEmbed.h \

SOURCES += PartGraph.cpp \
    PartCurve.cpp \
    PartSheet.cpp \
    PartLink.cpp \

# Graph visualization
SOURCES += QGraphViz/svgview.cpp
HEADERS += QGraphViz/svgview.h
