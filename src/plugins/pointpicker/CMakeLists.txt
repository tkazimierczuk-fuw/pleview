# This file should be called from the parent CMakeLists.txt

SET( pointpicker_SRCS
    pointpicker.cpp
    )

QT4_WRAP_CPP(pointpicker_MOC_SRCS pointpicker.h)

ADD_LIBRARY(pointpicker SHARED ${pointpicker_SRCS} ${pointpicker_MOC_SRCS})

IF(WIN32)
TARGET_LINK_LIBRARIES(pointpicker qwt-tk pleview muparser)
ENDIF(WIN32)

INSTALL(TARGETS pointpicker DESTINATION bin/plugins)
