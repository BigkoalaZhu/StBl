include($$[STARLAB])
include($$[SURFACEMESH])
include($$[CHOLMOD])

include($$[NANOFLANN])
include($$[OCTREE])
StarlabTemplate(plugin)

# Build flag
CONFIG(debug, debug|release) {
    CFG = debug
} else {
    CFG = release
}

# PartGraph library
LIBS += -L$$PWD/../PartGraphLib/$$CFG/lib -lPartGraphLib
INCLUDEPATH += ../PartGraphLib

# UtilityLib library
LIBS += -L$$PWD/../UtilityLib/$$CFG -lUtilityLib
INCLUDEPATH += ../UtilityLib

# PlausibilityAnalysis library
LIBS += -L$$PWD/../PlausibilityAnalysis/$$CFG/lib -lPlausibilityAnalysis
INCLUDEPATH += ../PlausibilityAnalysis

# OpenCV
CONFIG(debug, debug|release) {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300d -lopencv_world300d
} else {
    LIBS += -L$(OPENCV_LIB) -lopencv_ts300 -lopencv_world300
}
INCLUDEPATH += $(OPENCV_INC)

# LIBIGL
INCLUDEPATH += $(LIBIGL)

HEADERS += \
    structureblending_mode.h \
    stbl_widget.h
SOURCES += \
    structureblending_mode.cpp \
    stbl_widget.cpp

RESOURCES += structureblending_mode.qrc

FORMS += \
    stbl_widget.ui
