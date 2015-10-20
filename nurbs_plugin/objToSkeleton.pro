include($$[STARLAB])
include($$[SURFACEMESH])

include($$[NANOFLANN])
StarlabTemplate(app)

#CONFIG *= console
TARGET = objToSkeleton

# Build flag
CONFIG(debug, debug|release) {
    CFG = debug
} else {
    CFG = release
}

# NURBS library
LIBS += -L$$PWD/../ExternalTools/NURBS/$$CFG/lib -lNURBS
INCLUDEPATH += ../ExternalTools/NURBS

# Surface Reconstruction library
LIBS += -L$$PWD/../ExternalTools/Reconstruction/$$CFG/lib -lReconstruction
INCLUDEPATH += ../ExternalTools/Reconstruction

# Splat Rendering library
LIBS += -L$$PWD/../ExternalTools/GlSplatRendererLib/$$CFG/lib -lGlSplatRendererLib
INCLUDEPATH += ../ExternalTools/GlSplatRendererLib

# PartGraph library
LIBS += -L$$PWD/../PartGraphLib/$$CFG/lib -lPartGraphLib
INCLUDEPATH += ../PartGraphLib

SOURCES +=  objToSkeleton.cpp
HEADERS += objToSkeleton.h

mac:LIBS += -framework CoreFoundation # We need this for GLee

DESTDIR = $$PWD/$$CFG
