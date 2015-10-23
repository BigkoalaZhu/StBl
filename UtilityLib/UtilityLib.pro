include($$[STARLAB])
include($$[SURFACEMESH])

QT += gui opengl xml svg

TEMPLATE = lib
CONFIG += dynamiclib

# Build options
CONFIG(debug, debug|release) {CFG = debug} else {CFG = release}

# rapid
#LIBS += -L$$PWD/rapid-2013/lib/$$CFG/ -lRAPID
#INCLUDEPATH += rapid-2013/inc

HEADERS += \
    QuickMeshDraw.h \
    Sampler.h \
    OrientHelper.h \
    SegMeshLoader.h \
    UtilityGlobal.h \
	Colormap.h \
	writeOBJ.h \
	
SOURCES += \
    Sampler.cpp \	
    OrientHelper.cpp \
    SegMeshLoader.cpp \
    UtilityGlobal.cpp \
 
FORMS += \

RESOURCES += \
