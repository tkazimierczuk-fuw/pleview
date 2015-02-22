# This file should be called from the parent CMakeLists.txt

SET( peakfinder_SRCS
    peakfinder.cpp
    )

QT4_WRAP_CPP(peakfinder_MOC_SRCS peakfinder.h)

ADD_LIBRARY(peakfinder SHARED ${peakfinder_SRCS} ${peakfinder_MOC_SRCS})

IF(WIN32)
TARGET_LINK_LIBRARIES(peakfinder qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS peakfinder DESTINATION bin/plugins)
