# This file should be called from the parent CMakeLists.txt

SET( transpose_SRCS
    transposefilter.cpp
    )

QT4_WRAP_CPP(transpose_MOC_SRCS transposefilter.h)
ADD_LIBRARY(transpose SHARED ${transpose_SRCS} ${transpose_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(transpose qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS transpose   DESTINATION bin/plugins)
