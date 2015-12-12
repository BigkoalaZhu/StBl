include($$[STARLAB])
include($$[SURFACEMESH])
include($$[CHOLMOD])

include($$[NANOFLANN])
include($$[OCTREE])

QT += gui opengl xml svg

TEMPLATE = lib
CONFIG += staticlib

# Build options
CONFIG(debug, debug|release) {CFG = debug} else {CFG = release}

# UtilityLib library
LIBS += -L$$PWD/../UtilityLib/$$CFG -lUtilityLib
INCLUDEPATH += ../UtilityLib

# NURBS library
LIBS += -L$$PWD/../NURBS/$$CFG/lib -lNURBS
INCLUDEPATH += ../NURBS

# Structure Graph Library
LIBS += -L$$PWD/../StructureGraphLib/$$CFG/lib -lStructureGraphLib
INCLUDEPATH += ../StructureGraphLib

HEADERS += \
    CorrFinder.h \
	SegGraph.h \
	BoundaryFitting.h \
	BoundaryFittingGlobal.h \
	nurbs_global.h \
	
SOURCES += \
	CorrFinder.cpp \
	SegGraph.cpp \
	BoundaryFitting.cpp \
	
FORMS += \

RESOURCES += \
