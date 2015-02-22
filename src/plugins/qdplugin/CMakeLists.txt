# This file should be called from the parent CMakeLists.txt

INCLUDE_DIRECTORIES( ../xsectionfitter )

SET( qdplugin_SRCS
    qdplugin.cpp
    ../xsectionfitter/lmmin.cpp
    ../xsectionfitter/nonlinleastsq.cpp
    )

QT4_WRAP_CPP(qdplugin_MOC_SRCS qdplugin.h)

ADD_LIBRARY(qdplugin SHARED ${qdplugin_SRCS} ${qdplugin_MOC_SRCS})

IF(WIN32)
TARGET_LINK_LIBRARIES(qdplugin qwt-tk pleview muparser)
ENDIF(WIN32)

INSTALL(TARGETS qdplugin DESTINATION bin/plugins)
