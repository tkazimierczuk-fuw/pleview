# This file should be called from the parent CMakeLists.txt

# Assume QT is already turned on

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

INCLUDE_DIRECTORIES( . )

#ADD_SUBDIRECTORY(zeemanplugin)
#ADD_SUBDIRECTORY(mapper)
#ADD_SUBDIRECTORY(xsectionfitter)
#ADD_SUBDIRECTORY(simulator)
#ADD_SUBDIRECTORY(comment)
#ADD_SUBDIRECTORY(spikeremove)
#ADD_SUBDIRECTORY(transformer)
#ADD_SUBDIRECTORY(transpose)
#ADD_SUBDIRECTORY(variabledef)
#ADD_SUBDIRECTORY(differentiate)
#ADD_SUBDIRECTORY(peakfinder)
#ADD_SUBDIRECTORY(compareother)
#ADD_SUBDIRECTORY(qdplugin)
#ADD_SUBDIRECTORY(pointpicker)
#ADD_SUBDIRECTORY(foldfilter)
