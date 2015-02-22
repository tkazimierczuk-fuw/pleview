# This file should be called from the parent CMakeLists.txt

# Assume QT is already turned on

SET( mapperplugin_SRCS
    mapperplugin.cpp
    )

QT4_WRAP_CPP(mapperplugin_MOC_SRCS mapperplugin.h)
ADD_LIBRARY(mapperplugin SHARED ${mapperplugin_SRCS} ${mapperplugin_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(mapperplugin qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS mapperplugin DESTINATION bin/plugins)
