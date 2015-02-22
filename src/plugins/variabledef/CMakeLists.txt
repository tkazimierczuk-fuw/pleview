# This file should be called from the parent CMakeLists.txt

SET( rangevariablefilter_SRCS
    ../parserxml.cpp
    rangevariablefilter.cpp
    )

QT4_WRAP_CPP(rangevariablefilter_MOC_SRCS rangevariablefilter.h)
ADD_LIBRARY(rangevariablefilter SHARED ${rangevariablefilter_SRCS} ${rangevariablefilter_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(rangevariablefilter pleview muparser qwt-tk)
ENDIF(WIN32)

INSTALL(TARGETS rangevariablefilter   DESTINATION bin/plugins)
