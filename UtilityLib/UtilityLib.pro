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

# rapid
#LIBS += -L$$PWD/rapid-2013/lib/$$CFG/ -lRAPID
#INCLUDEPATH += rapid-2013/inc

# OpenCV
CONFIG(debug, debug|release) {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300d -lopencv_world300d
} else {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300 -lopencv_world300
}
INCLUDEPATH += $(OPENCV_INC)

# LIBIGL
INCLUDEPATH += $(LIBIGL)

# Liblbfgs
#LIBS += -L$$PWD/Liblbfgs/lib/$$CFG/ -llbfgs
#INCLUDEPATH += Liblbfgs

HEADERS += \
    QuickMeshDraw.h \
    Sampler.h \
    OrientHelper.h \
    SegMeshLoader.h \
    UtilityGlobal.h \
	Colormaps.h \
	writeOBJ.h \
	GenerateProjectedImage.h \
	IsotropicRemesher.h \
	ShapeNetFormat.h \
	emdL1.h \
	OBB_Volume.h \
	OBB_Volume_math.h \
	
SOURCES += \
    Sampler.cpp \	
    OrientHelper.cpp \
    SegMeshLoader.cpp \
    UtilityGlobal.cpp \
	GenerateProjectedImage.cpp \
	emdL1.cpp \
 
FORMS += \

RESOURCES += \
