# This file should be called from the parent CMakeLists.txt

SET( transformer_SRCS
    ../parserxml.cpp
    spectrumtransformer.cpp
    )

QT4_WRAP_CPP(transformer_MOC_SRCS spectrumtransformer.h)
ADD_LIBRARY(transformer SHARED ${transformer_SRCS} ${transformer_MOC_SRCS})
IF(WIN32)
TARGET_LINK_LIBRARIES(transformer qwt-tk pleview muparser)
ENDIF(WIN32)


INSTALL(TARGETS transformer  DESTINATION bin/plugins)
