# This file should be called from the parent CMakeLists.txt

SET( zeemanplugin_SRCS
    ../plotcurveext.cpp
    ../curvegroup.cpp
    linleastsq.cpp
    zeemanplugin.cpp
    ../xsectionfitter/lmmin.cpp
    ../xsectionfitter/nonlinleastsq.cpp
    )

ADD_LIBRARY(zeemanplugin SHARED ${zeemanplugin_SRCS} ${zeemanplugin_MOC_SRCS})
IF(WIN32)
  TARGET_LINK_LIBRARIES(zeemanplugin qwt-tk pleview muparser)
ENDIF(WIN32)


INSTALL(TARGETS zeemanplugin DESTINATION bin/plugins)
