# This file should be called from the parent CMakeLists.txt

SET( simulatorplugin_SRCS
    simulatorplugin.cpp
    ../curvegroup.cpp
    ../plotcurveext.cpp
    )

QT4_WRAP_CPP(simulatorplugin_MOC_SRCS simulatorplugin.h)
ADD_LIBRARY(simulatorplugin SHARED ${simulatorplugin_SRCS} ${simulatorplugin_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(simulatorplugin qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS simulatorplugin  DESTINATION bin/plugins)
