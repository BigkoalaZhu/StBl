include($$[STARLAB])
include($$[SURFACEMESH])
StarlabTemplate(none)

TEMPLATE = lib
CONFIG += dynamiclib
QT += opengl

# Build flag
CONFIG(debug, debug|release) {
    CFG = debug
} else {
    CFG = release
}

# Library name and destination
TARGET = BiSHDistance
DESTDIR = $$PWD/$$CFG/lib

# OpenCV
CONFIG(debug, debug|release) {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300d -lopencv_world300d
} else {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300 -lopencv_world300
}
INCLUDEPATH += $(OPENCV_INC)

HEADERS += \
    HausdorffDist.h \
	HausdorffImageMatch.h \
	HausdorffImageProcessor.h \
	HausdorffImageSimplify.h \
	HausdorffNode.h \
	SColorMap.h \
	utility.h \

SOURCES += \
    HausdorffDist.cpp \
	HausdorffImageMatch.cpp \
	HausdorffImageProcessor.cpp \
	HausdorffImageSimplify.cpp \
	SColorMap.cpp \
	utility.cpp \
