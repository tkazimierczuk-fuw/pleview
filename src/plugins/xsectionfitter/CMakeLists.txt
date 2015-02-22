# This file should be called from the parent CMakeLists.txt

SET( xsectionfitter_SRCS
    xsectionfitter.cpp
    lmmin.cpp
    nonlinleastsq.cpp
    variablewidget.cpp
    signalmerger.cpp
    sectionedcurve.cpp
    )

QT4_WRAP_CPP(xsectionfitter_MOC_SRCS xsectionfitter.h variablewidget.h signalmerger.h)

ADD_LIBRARY(xsectionfitter SHARED ${xsectionfitter_SRCS} ${xsectionfitter_MOC_SRCS})

IF(WIN32)
TARGET_LINK_LIBRARIES(xsectionfitter qwt-tk pleview muparser)
ENDIF(WIN32)

INSTALL(TARGETS xsectionfitter DESTINATION bin/plugins)
