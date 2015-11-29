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

HEADERS += \
    CorrFinder.h \
	
SOURCES += \
	CorrFinder.cpp \
	
FORMS += \

RESOURCES += \
