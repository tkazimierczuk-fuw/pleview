# This file should be called from the parent CMakeLists.txt

SET( comment_SRCS
    comment.cpp
    )

QT4_WRAP_CPP(comment_MOC_SRCS comment.h)
ADD_LIBRARY(comment SHARED ${comment_SRCS} ${comment_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(comment pleview qwt-tk)
ENDIF(WIN32)


INSTALL(TARGETS comment DESTINATION bin/plugins)
