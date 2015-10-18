include($$[STARLAB])
include($$[SURFACEMESH])
include($$[CHOLMOD])
StarlabTemplate(plugin)

# Build flag
CONFIG(debug, debug|release) {
    CFG = debug
} else {
    CFG = release
}

# StructureGraph library
# LIBS += -L$$PWD/../StructureGraphLib/$$CFG/lib -lStructureGraphLib
# INCLUDEPATH += ../StructureGraphLib

# UtilityLib library
LIBS += -L$$PWD/../UtilityLib/$$CFG -lUtilityLib
INCLUDEPATH += ../UtilityLib

# ThreadsGraphLib library
#LIBS += -L$$PWD/../ThreadsGraphLib/$$CFG -lThreadsGraphLib
#INCLUDEPATH += ../ThreadsGraphLib

HEADERS += \
    structureblending_mode.h \
    stbl_widget.h
SOURCES += \
    structureblending_mode.cpp \
    stbl_widget.cpp

RESOURCES += structureblending_mode.qrc

FORMS += \
    stbl_widget.ui
