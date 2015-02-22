# This file should be called from the parent CMakeLists.txt

SET( differentiate_SRCS
    ../parserxml.cpp
    differentialmodel.cpp
    differentiate.cpp
    )

QT4_WRAP_CPP(differentiate_MOC_SRCS differentiate.h differentialmodel.h)
ADD_LIBRARY(differential SHARED ${differentiate_SRCS} ${differentiate_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(differential qwt-tk pleview muparser)
ENDIF(WIN32)


INSTALL(TARGETS differential DESTINATION bin/plugins)
