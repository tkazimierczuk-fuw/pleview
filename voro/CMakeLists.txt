PROJECT(voro)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8.11)

OPTION(USEQWT "Include VoronoiItem for QwtPlot" OFF)


SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

IF(UNIX)
  SET(CMAKE_CXX_FLAGS "-std=c++11")
ENDIF(UNIX)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)

SET(SRCS_voro
${voro_SOURCE_DIR}/common.cc
${voro_SOURCE_DIR}/cell_2d.cc
${voro_SOURCE_DIR}/c_loops_2d.cc
${voro_SOURCE_DIR}/v_base_2d.cc
${voro_SOURCE_DIR}/container_2d.cc
${voro_SOURCE_DIR}/v_compute_2d.cc
${voro_SOURCE_DIR}/wall_2d.cc
${voro_SOURCE_DIR}/cell_nc_2d.cc
${voro_SOURCE_DIR}/ctr_boundary_2d.cc
${voro_SOURCE_DIR}/vorowrapper.cpp
)

SET(LIBS_voro Qt5::Core)

#IF(USEQWT)
  SET(SRCS_voro ${SRCS_voro} voronoi_plot_item.cpp)
  SET(LIBS_voro ${LIBS_voro} qwt-tk)
  ADD_DEFINITIONS("-DUSEQWT")
#ENDIF(USEQWT)

message(${SRCS_voro})

ADD_EXECUTABLE(examples ${SRCS_voro} examples.cpp)
TARGET_LINK_LIBRARIES(examples ${LIBS_voro} Qt5::Gui)


ADD_LIBRARY(voro ${SRCS_voro})
TARGET_LINK_LIBRARIES(voro ${LIBS_voro})
