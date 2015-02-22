# This file should be called from the parent CMakeLists.txt

SET( spikeremove_SRCS
    spikeremove.cpp
    )

QT4_WRAP_CPP(spikeremove_MOC_SRCS spikeremove.h)
ADD_LIBRARY(spikeremove SHARED ${spikeremove_SRCS} ${spikeremove_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(spikeremove pleview qwt-tk)
ENDIF(WIN32)

INSTALL(TARGETS spikeremove DESTINATION bin/plugins)
