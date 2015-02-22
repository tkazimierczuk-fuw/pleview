# This file should be called from the parent CMakeLists.txt

SET( compareother_SRCS
    compareother.cpp
    doublemapitem.cpp
    )

QT4_WRAP_CPP(compareother_MOC_SRCS compareother.h doublemapitem.h)

ADD_LIBRARY(compareother SHARED ${compareother_SRCS} ${compareother_MOC_SRCS})

IF(WIN32)
TARGET_LINK_LIBRARIES(compareother qwt-tk pleview)
ENDIF(WIN32)

INSTALL(TARGETS compareother DESTINATION bin/plugins)
