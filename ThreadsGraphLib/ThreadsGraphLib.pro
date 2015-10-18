include($$[STARLAB])
include($$[SURFACEMESH])

include($$[NANOFLANN])
include($$[OCTREE])
include($$[CHOLMOD])

QT += opengl xml svg concurrent

TEMPLATE = lib
CONFIG += staticlib

# Build options
CONFIG(debug, debug|release) {CFG = debug} else {CFG = release}

# NURBS library
LIBS += -L$$PWD/../ExternalTools/NURBS/$$CFG/lib -lNURBS
INCLUDEPATH += ../ExternalTools/NURBS

# Splat Rendering library
LIBS += -L$$PWD/../ExternalTools/GlSplatRendererLib/$$CFG/lib -lGlSplatRendererLib
INCLUDEPATH += ../ExternalTools/GlSplatRendererLib

# UtilityLib library
LIBS += -L$$PWD/../UtilityLib/$$CFG -lUtilityLib
INCLUDEPATH += ../UtilityLib

# Graph visualization
SOURCES += QGraphViz/svgview.cpp
HEADERS += QGraphViz/svgview.h

HEADERS += \
    ThreadsGraphGlobal.h \
    ThreadsGraphNode.h \
	ThreadsGraphSheet.h \
	ThreadsGraphCurve.h \
    ThreadsGraphLink.h \
    ThreadsGraph.h \
	ThreadsGraphMatching.h \
	
SOURCES += \
    ThreadsGraphSheet.cpp \
	ThreadsGraphCurve.cpp \
    ThreadsGraphNode.cpp \
    ThreadsGraphLink.cpp \
    ThreadsGraph.cpp \
	ThreadsGraphMatching.cpp \
 
FORMS += \

RESOURCES += \
