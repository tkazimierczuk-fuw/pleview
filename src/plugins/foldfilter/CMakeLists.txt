# This file should be called from the parent CMakeLists.txt

SET( foldfilter_SRCS
    foldfilter.cpp
    )

QT4_WRAP_CPP(foldfilter_MOC_SRCS foldfilter.h)
ADD_LIBRARY(foldfilter SHARED ${foldfilter_SRCS} ${foldfilter_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(foldfilter qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS foldfilter DESTINATION bin/plugins)
