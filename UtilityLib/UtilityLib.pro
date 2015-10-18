include($$[STARLAB])
include($$[SURFACEMESH])

QT += gui opengl xml svg

TEMPLATE = lib
CONFIG += staticlib

# Build options
CONFIG(debug, debug|release) {CFG = debug} else {CFG = release}

# rapid
LIBS += -L$$PWD/rapid-2013/lib/$$CFG/ -lRAPID
INCLUDEPATH += rapid-2013/inc

HEADERS += \
    Detect_collision.h \
    QuickMeshDraw.h \
    Sampler.h \
    OrientHelper.h \
    SegMeshLoader.h \
    UtilityGlobal.h \
	Colormap.h \
	writeOBJ.h \
	
SOURCES += \
    Detect_collision.cpp \
    Sampler.cpp \	
    OrientHelper.cpp \
    SegMeshLoader.cpp \
    UtilityGlobal.cpp \
 
FORMS += \

RESOURCES += \
