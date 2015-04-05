PROJECT(qwt-tk)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

IF(UNIX)
  SET(CMAKE_CXX_FLAGS "-std=c++11")
ENDIF(UNIX)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)

INCLUDE_DIRECTORIES(${qwt-tk_SOURCE_DIR}/src ${qwt-tk_SOURCE_DIR}/src-tk)

SET(QWT_SRC_DIR ${qwt-tk_SOURCE_DIR}/src)
SET(QWTTK_SRC_DIR ${qwt-tk_SOURCE_DIR}/src-tk)


SET(QWT_MOC_HDRS
${QWT_SRC_DIR}/qwt_abstract_legend.h
${QWT_SRC_DIR}/qwt_abstract_scale_draw.h
${QWT_SRC_DIR}/qwt_clipper.h
${QWT_SRC_DIR}/qwt_color_map.h
${QWT_SRC_DIR}/qwt_column_symbol.h
${QWT_SRC_DIR}/qwt_compat.h
${QWT_SRC_DIR}/qwt_curve_fitter.h
${QWT_SRC_DIR}/qwt_date.h
${QWT_SRC_DIR}/qwt_date_scale_draw.h
${QWT_SRC_DIR}/qwt_date_scale_engine.h
${QWT_SRC_DIR}/qwt_dial_needle.h
${QWT_SRC_DIR}/qwt_dyngrid_layout.h
${QWT_SRC_DIR}/qwt_event_pattern.h
${QWT_SRC_DIR}/qwt_global.h
${QWT_SRC_DIR}/qwt_graphic.h
${QWT_SRC_DIR}/qwt.h
${QWT_SRC_DIR}/qwt_interval.h
${QWT_SRC_DIR}/qwt_interval_symbol.h
${QWT_SRC_DIR}/qwt_legend_data.h
${QWT_SRC_DIR}/qwt_legend.h
${QWT_SRC_DIR}/qwt_legend_label.h
${QWT_SRC_DIR}/qwt_magnifier.h
${QWT_SRC_DIR}/qwt_math.h
${QWT_SRC_DIR}/qwt_matrix_raster_data.h
${QWT_SRC_DIR}/qwt_null_paintdevice.h
${QWT_SRC_DIR}/qwt_painter_command.h
${QWT_SRC_DIR}/qwt_painter.h
${QWT_SRC_DIR}/qwt_panner.h
${QWT_SRC_DIR}/qwt_picker.h
${QWT_SRC_DIR}/qwt_picker_machine.h
${QWT_SRC_DIR}/qwt_pixel_matrix.h
${QWT_SRC_DIR}/qwt_plot_abstract_barchart.h
${QWT_SRC_DIR}/qwt_plot_barchart.h
${QWT_SRC_DIR}/qwt_plot_canvas.h
${QWT_SRC_DIR}/qwt_plot_curve.h
${QWT_SRC_DIR}/qwt_plot_dict.h
${QWT_SRC_DIR}/qwt_plot_directpainter.h
#${QWT_SRC_DIR}/qwt_plot_glcanvas.h
${QWT_SRC_DIR}/qwt_plot_grid.h
${QWT_SRC_DIR}/qwt_plot.h
${QWT_SRC_DIR}/qwt_plot_histogram.h
${QWT_SRC_DIR}/qwt_plot_intervalcurve.h
${QWT_SRC_DIR}/qwt_plot_item.h
${QWT_SRC_DIR}/qwt_plot_layout.h
${QWT_SRC_DIR}/qwt_plot_legenditem.h
${QWT_SRC_DIR}/qwt_plot_magnifier.h
${QWT_SRC_DIR}/qwt_plot_marker.h
${QWT_SRC_DIR}/qwt_plot_multi_barchart.h
${QWT_SRC_DIR}/qwt_plot_panner.h
${QWT_SRC_DIR}/qwt_plot_picker.h
${QWT_SRC_DIR}/qwt_plot_rasteritem.h
${QWT_SRC_DIR}/qwt_plot_renderer.h
${QWT_SRC_DIR}/qwt_plot_rescaler.h
${QWT_SRC_DIR}/qwt_plot_scaleitem.h
${QWT_SRC_DIR}/qwt_plot_seriesitem.h
${QWT_SRC_DIR}/qwt_plot_shapeitem.h
${QWT_SRC_DIR}/qwt_plot_spectrocurve.h
${QWT_SRC_DIR}/qwt_plot_spectrogram.h
${QWT_SRC_DIR}/qwt_plot_svgitem.h
${QWT_SRC_DIR}/qwt_plot_textlabel.h
${QWT_SRC_DIR}/qwt_plot_tradingcurve.h
${QWT_SRC_DIR}/qwt_plot_zoneitem.h
${QWT_SRC_DIR}/qwt_plot_zoomer.h
${QWT_SRC_DIR}/qwt_point_3d.h
${QWT_SRC_DIR}/qwt_point_data.h
${QWT_SRC_DIR}/qwt_point_mapper.h
${QWT_SRC_DIR}/qwt_point_polar.h
${QWT_SRC_DIR}/qwt_raster_data.h
${QWT_SRC_DIR}/qwt_round_scale_draw.h
${QWT_SRC_DIR}/qwt_samples.h
${QWT_SRC_DIR}/qwt_sampling_thread.h
${QWT_SRC_DIR}/qwt_scale_div.h
${QWT_SRC_DIR}/qwt_scale_draw.h
${QWT_SRC_DIR}/qwt_scale_engine.h
${QWT_SRC_DIR}/qwt_scale_map.h
${QWT_SRC_DIR}/qwt_scale_widget.h
${QWT_SRC_DIR}/qwt_series_data.h
${QWT_SRC_DIR}/qwt_series_store.h
${QWT_SRC_DIR}/qwt_spline.h
${QWT_SRC_DIR}/qwt_symbol.h
${QWT_SRC_DIR}/qwt_system_clock.h
${QWT_SRC_DIR}/qwt_text_engine.h
${QWT_SRC_DIR}/qwt_text.h
${QWT_SRC_DIR}/qwt_text_label.h
${QWT_SRC_DIR}/qwt_transform.h
${QWT_SRC_DIR}/qwt_widget_overlay.h
${QWTTK_SRC_DIR}/advancedplot.h
${QWTTK_SRC_DIR}/colorbutton.h
${QWTTK_SRC_DIR}/colormap.h
${QWTTK_SRC_DIR}/colormapslider.h
${QWTTK_SRC_DIR}/data1d.h
${QWTTK_SRC_DIR}/data2d.h
${QWTTK_SRC_DIR}/data.h
${QWTTK_SRC_DIR}/doublemarker.h
${QWTTK_SRC_DIR}/MapItem.h
${QWTTK_SRC_DIR}/offlinerenderitem.h
${QWTTK_SRC_DIR}/qwt_abstract_slider.h
${QWTTK_SRC_DIR}/timecounter.h
${QWTTK_SRC_DIR}/xml.h

)

SET(QWT_SRCS
${QWT_SRC_DIR}/qwt_abstract_legend.cpp
${QWT_SRC_DIR}/qwt_abstract_scale_draw.cpp
${QWT_SRC_DIR}/qwt_clipper.cpp
${QWT_SRC_DIR}/qwt_color_map.cpp
${QWT_SRC_DIR}/qwt_column_symbol.cpp
${QWT_SRC_DIR}/qwt_curve_fitter.cpp
${QWT_SRC_DIR}/qwt_date.cpp
${QWT_SRC_DIR}/qwt_date_scale_draw.cpp
${QWT_SRC_DIR}/qwt_date_scale_engine.cpp
${QWT_SRC_DIR}/qwt_dial_needle.cpp
${QWT_SRC_DIR}/qwt_dyngrid_layout.cpp
${QWT_SRC_DIR}/qwt_event_pattern.cpp
${QWT_SRC_DIR}/qwt_graphic.cpp
${QWT_SRC_DIR}/qwt_interval.cpp
${QWT_SRC_DIR}/qwt_interval_symbol.cpp
${QWT_SRC_DIR}/qwt_legend.cpp
${QWT_SRC_DIR}/qwt_legend_data.cpp
${QWT_SRC_DIR}/qwt_legend_label.cpp
${QWT_SRC_DIR}/qwt_magnifier.cpp
${QWT_SRC_DIR}/qwt_math.cpp
${QWT_SRC_DIR}/qwt_matrix_raster_data.cpp
${QWT_SRC_DIR}/qwt_null_paintdevice.cpp
${QWT_SRC_DIR}/qwt_painter_command.cpp
${QWT_SRC_DIR}/qwt_painter.cpp
${QWT_SRC_DIR}/qwt_panner.cpp
${QWT_SRC_DIR}/qwt_picker.cpp
${QWT_SRC_DIR}/qwt_picker_machine.cpp
${QWT_SRC_DIR}/qwt_pixel_matrix.cpp
${QWT_SRC_DIR}/qwt_plot_abstract_barchart.cpp
${QWT_SRC_DIR}/qwt_plot_axis.cpp
${QWT_SRC_DIR}/qwt_plot_barchart.cpp
${QWT_SRC_DIR}/qwt_plot_canvas.cpp
${QWT_SRC_DIR}/qwt_plot.cpp
${QWT_SRC_DIR}/qwt_plot_curve.cpp
${QWT_SRC_DIR}/qwt_plot_dict.cpp
${QWT_SRC_DIR}/qwt_plot_directpainter.cpp
#${QWT_SRC_DIR}/qwt_plot_glcanvas.cpp
${QWT_SRC_DIR}/qwt_plot_grid.cpp
${QWT_SRC_DIR}/qwt_plot_histogram.cpp
${QWT_SRC_DIR}/qwt_plot_intervalcurve.cpp
${QWT_SRC_DIR}/qwt_plot_item.cpp
${QWT_SRC_DIR}/qwt_plot_layout.cpp
${QWT_SRC_DIR}/qwt_plot_legenditem.cpp
${QWT_SRC_DIR}/qwt_plot_magnifier.cpp
${QWT_SRC_DIR}/qwt_plot_marker.cpp
${QWT_SRC_DIR}/qwt_plot_multi_barchart.cpp
${QWT_SRC_DIR}/qwt_plot_panner.cpp
${QWT_SRC_DIR}/qwt_plot_picker.cpp
${QWT_SRC_DIR}/qwt_plot_rasteritem.cpp
${QWT_SRC_DIR}/qwt_plot_renderer.cpp
${QWT_SRC_DIR}/qwt_plot_rescaler.cpp
${QWT_SRC_DIR}/qwt_plot_scaleitem.cpp
${QWT_SRC_DIR}/qwt_plot_seriesitem.cpp
${QWT_SRC_DIR}/qwt_plot_shapeitem.cpp
${QWT_SRC_DIR}/qwt_plot_spectrocurve.cpp
${QWT_SRC_DIR}/qwt_plot_spectrogram.cpp
${QWT_SRC_DIR}/qwt_plot_svgitem.cpp
${QWT_SRC_DIR}/qwt_plot_textlabel.cpp
${QWT_SRC_DIR}/qwt_plot_tradingcurve.cpp
${QWT_SRC_DIR}/qwt_plot_xml.cpp
${QWT_SRC_DIR}/qwt_plot_zoneitem.cpp
${QWT_SRC_DIR}/qwt_plot_zoomer.cpp
${QWT_SRC_DIR}/qwt_point_3d.cpp
${QWT_SRC_DIR}/qwt_point_data.cpp
${QWT_SRC_DIR}/qwt_point_mapper.cpp
${QWT_SRC_DIR}/qwt_point_polar.cpp
${QWT_SRC_DIR}/qwt_raster_data.cpp
${QWT_SRC_DIR}/qwt_round_scale_draw.cpp
${QWT_SRC_DIR}/qwt_sampling_thread.cpp
${QWT_SRC_DIR}/qwt_scale_div.cpp
${QWT_SRC_DIR}/qwt_scale_draw.cpp
${QWT_SRC_DIR}/qwt_scale_engine.cpp
${QWT_SRC_DIR}/qwt_scale_map.cpp
${QWT_SRC_DIR}/qwt_scale_widget.cpp
${QWT_SRC_DIR}/qwt_series_data.cpp
${QWT_SRC_DIR}/qwt_spline.cpp
${QWT_SRC_DIR}/qwt_symbol.cpp
${QWT_SRC_DIR}/qwt_system_clock.cpp
${QWT_SRC_DIR}/qwt_text.cpp
${QWT_SRC_DIR}/qwt_text_engine.cpp
${QWT_SRC_DIR}/qwt_text_label.cpp
${QWT_SRC_DIR}/qwt_transform.cpp
${QWT_SRC_DIR}/qwt_widget_overlay.cpp
${QWTTK_SRC_DIR}/advancedplot.cpp
${QWTTK_SRC_DIR}/colorbutton.cpp
${QWTTK_SRC_DIR}/colormap.cpp
${QWTTK_SRC_DIR}/colormapslider.cpp
${QWTTK_SRC_DIR}/data1d.cpp
${QWTTK_SRC_DIR}/data2d.cpp
${QWTTK_SRC_DIR}/data.cpp
${QWTTK_SRC_DIR}/doublemarker.cpp
${QWTTK_SRC_DIR}/MapItem.cpp
${QWTTK_SRC_DIR}/offlinerenderitem.cpp
${QWTTK_SRC_DIR}/timecounter.cpp
${QWTTK_SRC_DIR}/xml.cpp

)




#ADD_EXECUTABLE(qwt-tk ${QWTTK_SRC_DIR}/main.cpp ${QWT_SRCS} ${QWT_MOC_SRCS})
ADD_LIBRARY(qwt-tk ${QWT_SRCS} ${QWTTK_SRCS} ${QWT_MOC_SRCS})
TARGET_LINK_LIBRARIES(qwt-tk Qt5::Widgets Qt5::Svg Qt5::PrintSupport Qt5::Concurrent Qt5::Xml)
