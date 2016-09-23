cmake_minimum_required(VERSION 3.0)

project(qwt4pleview)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5Svg_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})

set(QWT_SRCS
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_legend.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_scale.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_scale_draw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_abstract_slider.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_analog_clock.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_arrow_button.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_clipper.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_color_map.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_column_symbol.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_compass.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_compass_rose.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_counter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_curve_fitter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date_scale_draw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_date_scale_engine.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dial.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dial_needle.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_dyngrid_layout.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_event_pattern.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_graphic.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_interval.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_interval_symbol.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_knob.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend_data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_legend_label.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_magnifier.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_math.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_matrix_raster_data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_null_paintdevice.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_painter_command.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_painter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_panner.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_picker.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_picker_machine.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_pixel_matrix.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_abstract_barchart.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_axis.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_barchart.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_canvas.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_curve.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_dict.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_directpainter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_grid.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_histogram.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_intervalcurve.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_item.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_layout.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_legenditem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_magnifier.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_marker.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_multi_barchart.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_panner.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_picker.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_rasteritem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_renderer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_rescaler.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_scaleitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_seriesitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_shapeitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_spectrocurve.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_spectrogram.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_svgitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_textlabel.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_tradingcurve.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_xml.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_zoneitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_zoomer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_3d.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_mapper.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_point_polar.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_raster_data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_round_scale_draw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_sampling_thread.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_div.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_draw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_engine.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_map.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_scale_widget.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_series_data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_slider.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_spline.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_symbol.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_system_clock.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text_engine.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_text_label.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_thermo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_transform.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_wheel.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_widget_overlay.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qwt/src/qwt_plot_glcanvas.cpp  
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qwt/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(MOD_SRCS
 ${CMAKE_CURRENT_SOURCE_DIR}/src/advancedplot.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/colormap.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/data1d.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/data.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/offlinerenderitem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/xml.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/colorbutton.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/colormapslider.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/data2d.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/doublemarker.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/MapItem.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/timecounter.cpp
)

add_library(qwt4pleview ${QWT_SRCS} ${MOD_SRCS})
target_link_libraries(qwt4pleview Qt5::Widgets Qt5::PrintSupport Qt5::Svg Qt5::OpenGL Qt5::Xml)

add_executable(test_colormapslider ${CMAKE_CURRENT_SOURCE_DIR}/examples/test_colormapslider.cpp)
target_link_libraries(test_colormapslider qwt4pleview  Qt5::Widgets)

add_executable(test_saving_to_xml ${CMAKE_CURRENT_SOURCE_DIR}/examples/test_saving_to_xml.cpp)
target_link_libraries(test_saving_to_xml qwt4pleview  Qt5::Widgets)
