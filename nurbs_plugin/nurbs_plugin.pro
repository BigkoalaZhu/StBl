include($$[STARLAB])
include($$[SURFACEMESH])
include($$[CHOLMOD])

include($$[NANOFLANN])
StarlabTemplate(plugin)

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

HEADERS +=  nurbs_plugin.h \
            nurbstools.h \
            OBB_Volume.h \
            PCA3.h \
            LSCM.h \
            BoundaryFitting.h \
			GraphModifyWidget.h \

SOURCES +=  nurbs_plugin.cpp \
            nurbstools.cpp \
            BoundaryFitting.cpp \
			GraphModifyWidget.cpp \
	
RESOURCES += nurbs_plugin.qrc

FORMS += nurbstools.ui GraphModifyWidget.ui

mac:LIBS += -framework CoreFoundation # We need this for GLee
