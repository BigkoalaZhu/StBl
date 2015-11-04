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
TARGET = PlausibilityAnalysis
DESTDIR = $$PWD/$$CFG/lib

# OpenCV
CONFIG(debug, debug|release) {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300d -lopencv_world300d
} else {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300 -lopencv_world300
}
INCLUDEPATH += $(OPENCV_INC)

# UtilityLib library
LIBS += -L$$PWD/../UtilityLib/$$CFG -lUtilityLib
INCLUDEPATH += ../UtilityLib

# BiSHDistance library
LIBS += -L$$PWD/../BiSHDistance/$$CFG/lib -lBiSHDistance
INCLUDEPATH += ../BiSHDistance

HEADERS += PlausibilityDistance.h \

SOURCES += PlausibilityDistance.cpp \
