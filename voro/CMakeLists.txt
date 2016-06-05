PROJECT(voro)

cmake_minimum_required(VERSION 3.0)

project(qwt4pleview)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})


OPTION(USEQWT "Include VoronoiItem for QwtPlot" OFF)


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
  SET(LIBS_voro ${LIBS_voro} qwt4pleview)
  ADD_DEFINITIONS("-DUSEQWT")
#ENDIF(USEQWT)

message(${SRCS_voro})

ADD_EXECUTABLE(examples ${SRCS_voro} examples.cpp)
TARGET_LINK_LIBRARIES(examples ${LIBS_voro} Qt5::Gui)


ADD_LIBRARY(voro ${SRCS_voro})
TARGET_LINK_LIBRARIES(voro ${LIBS_voro})
