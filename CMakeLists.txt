PROJECT(pleview)

CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

IF(UNIX)
SET(CMAKE_CXX_FLAGS "-std=c++11")
ENDIF(UNIX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

SET(CMAKE_BUILD_TYPE Release)

SET(QT_USE_QTXML true)
SET(QT_USE_QTSVG true)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})


find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})



INCLUDE_DIRECTORIES(
    ${pleview_SOURCE_DIR}/src
    ${pleview_SOURCE_DIR}/qwt-tk/src
    ${pleview_SOURCE_DIR}/qwt-tk/src-tk
    ${pleview_SOURCE_DIR}/muparser/include
    ${pleview_SOURCE_DIR}/zlib/
    ${pleview_SOURCE_DIR}/src/plugins/
${pleview_SOURCE_DIR}/src/plugins/zeemanplugin
${pleview_SOURCE_DIR}/src/plugins/mapper
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter
${pleview_SOURCE_DIR}/src/plugins/simulator
${pleview_SOURCE_DIR}/src/plugins/comment
${pleview_SOURCE_DIR}/src/plugins/spikeremove
${pleview_SOURCE_DIR}/src/plugins/transformer
${pleview_SOURCE_DIR}/src/plugins/transpose
${pleview_SOURCE_DIR}/src/plugins/variabledef
${pleview_SOURCE_DIR}/src/plugins/differentiate
${pleview_SOURCE_DIR}/src/plugins/peakfinder
${pleview_SOURCE_DIR}/src/plugins/compareother
${pleview_SOURCE_DIR}/src/plugins/qdplugin
${pleview_SOURCE_DIR}/src/plugins/pointpicker
${pleview_SOURCE_DIR}/src/plugins/foldfilter
)

ADD_SUBDIRECTORY(qwt-tk)
ADD_SUBDIRECTORY(muparser)
ADD_SUBDIRECTORY(zlib)


SET(PLEVIEW_VERSION_MAJOR "0")
SET(PLEVIEW_VERSION_MINOR "pre19")
SET(PLEVIEW_VERSION "${PLEVIEW_VERSION_MAJOR}.${PLEVIEW_VERSION_MINOR}")
ADD_DEFINITIONS("-DPLEVIEW_VERSION=\"${PLEVIEW_VERSION}\"")

SET(MOC_HDRS
  ${pleview_SOURCE_DIR}/src/mainwindow.h
  ${pleview_SOURCE_DIR}/src/axis.h
  ${pleview_SOURCE_DIR}/src/model.h
  ${pleview_SOURCE_DIR}/src/parserwidget.h
  ${pleview_SOURCE_DIR}/src/plotwithmarker.h
  ${pleview_SOURCE_DIR}/src/mapwithmarker.h
  ${pleview_SOURCE_DIR}/src/configdialog.h
  ${pleview_SOURCE_DIR}/src/plugins.h
  ${pleview_SOURCE_DIR}/src/datafilters.h
  ${pleview_SOURCE_DIR}/src/plotaddonsgui.h
  ${pleview_SOURCE_DIR}/src/datafiltersgui.h
  ${pleview_SOURCE_DIR}/src/plotaddons.h
  ${pleview_SOURCE_DIR}/src/qtiocompressor.h
  ${pleview_SOURCE_DIR}/src/log.h
  ${pleview_SOURCE_DIR}/src/dataexport.h
  ${pleview_SOURCE_DIR}/src/variable.h
  ${pleview_SOURCE_DIR}/src/plotrangesmanager.h
  ${pleview_SOURCE_DIR}/src/axistransformation.h
)

SET(plugin_HDRS
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/nonlinleastsq.h
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/xsectionfitter.h
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/lmmin.h
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/variablewidget.h
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/sectionedcurve.h
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/signalmerger.h
${pleview_SOURCE_DIR}/src/plugins/transpose/transposefilter.h
${pleview_SOURCE_DIR}/src/plugins/simulator/simulatorplugin.h
${pleview_SOURCE_DIR}/src/plugins/variabledef/rangevariablefilter.h
${pleview_SOURCE_DIR}/src/plugins/comment/comment.h
${pleview_SOURCE_DIR}/src/plugins/peakfinder/peakfinder.h
${pleview_SOURCE_DIR}/src/plugins/plotcurveext.h
${pleview_SOURCE_DIR}/src/plugins/qdplugin/qdplugin.h
${pleview_SOURCE_DIR}/src/plugins/pointpicker/pointpicker.h
${pleview_SOURCE_DIR}/src/plugins/mapper/mapperplugin.h
${pleview_SOURCE_DIR}/src/plugins/curvegroup.h
${pleview_SOURCE_DIR}/src/plugins/transformer/spectrumtransformer.h
${pleview_SOURCE_DIR}/src/plugins/differentiate/differentiate.h
${pleview_SOURCE_DIR}/src/plugins/differentiate/differentialmodel.h
${pleview_SOURCE_DIR}/src/plugins/compareother/compareother.h
${pleview_SOURCE_DIR}/src/plugins/compareother/doublemapitem.h
${pleview_SOURCE_DIR}/src/plugins/spikeremove/spikeremove.h
${pleview_SOURCE_DIR}/src/plugins/foldfilter/foldfilter.h
${pleview_SOURCE_DIR}/src/plugins/parserxml.h
${pleview_SOURCE_DIR}/src/plugins/zeemanplugin/linleastsq.h
${pleview_SOURCE_DIR}/src/plugins/zeemanplugin/zeemanplugin.h
)


SET(Pleview_SRCS
  ${pleview_SOURCE_DIR}/src/mainwindow.cpp
  ${pleview_SOURCE_DIR}/src/axis.cpp
  ${pleview_SOURCE_DIR}/src/model.cpp
  ${pleview_SOURCE_DIR}/src/parserwidget.cpp
  ${pleview_SOURCE_DIR}/src/plotwithmarker.cpp
  ${pleview_SOURCE_DIR}/src/mapwithmarker.cpp
  ${pleview_SOURCE_DIR}/src/main.cpp
  ${pleview_SOURCE_DIR}/src/configdialog.cpp
  ${pleview_SOURCE_DIR}/src/plugins.cpp
  ${pleview_SOURCE_DIR}/src/plotaddons.cpp
  ${pleview_SOURCE_DIR}/src/datafilters.cpp
  ${pleview_SOURCE_DIR}/src/datafiltersgui.cpp
  ${pleview_SOURCE_DIR}/src/plotaddonsgui.cpp
  ${pleview_SOURCE_DIR}/src/qtiocompressor.cpp
  ${pleview_SOURCE_DIR}/src/dataexport.cpp
  ${pleview_SOURCE_DIR}/src/dataimport.cpp
  ${pleview_SOURCE_DIR}/src/dataimport_hamamatsu.cpp
  ${pleview_SOURCE_DIR}/src/dataimport_pumpprobe.cpp
  ${pleview_SOURCE_DIR}/src/log.cpp
  ${pleview_SOURCE_DIR}/src/pleview.cpp
  ${pleview_SOURCE_DIR}/src/variable.cpp
  ${pleview_SOURCE_DIR}/src/plotrangesmanager.cpp
  ${pleview_SOURCE_DIR}/src/axistransformation.cpp
  ${pleview_SOURCE_DIR}/src/vectorwidget.cpp
)


SET(plugin_SRCS
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/lmmin.cpp
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/sectionedcurve.cpp
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/signalmerger.cpp
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/variablewidget.cpp
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/nonlinleastsq.cpp
${pleview_SOURCE_DIR}/src/plugins/xsectionfitter/xsectionfitter.cpp
${pleview_SOURCE_DIR}/src/plugins/transpose/transposefilter.cpp
${pleview_SOURCE_DIR}/src/plugins/plotcurveext.cpp
${pleview_SOURCE_DIR}/src/plugins/simulator/simulatorplugin.cpp
${pleview_SOURCE_DIR}/src/plugins/curvegroup.cpp
${pleview_SOURCE_DIR}/src/plugins/variabledef/rangevariablefilter.cpp
${pleview_SOURCE_DIR}/src/plugins/comment/comment.cpp
${pleview_SOURCE_DIR}/src/plugins/peakfinder/peakfinder.cpp
${pleview_SOURCE_DIR}/src/plugins/qdplugin/qdplugin.cpp
${pleview_SOURCE_DIR}/src/plugins/parserxml.cpp
${pleview_SOURCE_DIR}/src/plugins/pointpicker/pointpicker.cpp
${pleview_SOURCE_DIR}/src/plugins/mapper/mapperplugin.cpp
${pleview_SOURCE_DIR}/src/plugins/transformer/spectrumtransformer.cpp
${pleview_SOURCE_DIR}/src/plugins/differentiate/differentialmodel.cpp
${pleview_SOURCE_DIR}/src/plugins/differentiate/differentiate.cpp
${pleview_SOURCE_DIR}/src/plugins/compareother/doublemapitem.cpp
${pleview_SOURCE_DIR}/src/plugins/compareother/compareother.cpp
${pleview_SOURCE_DIR}/src/plugins/spikeremove/spikeremove.cpp
${pleview_SOURCE_DIR}/src/plugins/foldfilter/foldfilter.cpp
${pleview_SOURCE_DIR}/src/plugins/zeemanplugin/zeemanplugin.cpp
${pleview_SOURCE_DIR}/src/plugins/zeemanplugin/linleastsq.cpp
)


QT5_ADD_RESOURCES( RC_SRCS  ${pleview_SOURCE_DIR}/img/resources.qrc)

ADD_EXECUTABLE(pleview WIN32 ${Pleview_SRCS} ${RC_SRCS} ${pleview_SOURCE_DIR}/img/pleview.rc ${plugin_SRCS})
TARGET_LINK_LIBRARIES(pleview Qt5::Widgets Qt5::Svg Qt5::PrintSupport Qt5::Concurrent qwt-tk muparser zlib)


#ADD_SUBDIRECTORY(src/plugins)


INSTALL(TARGETS pleview
        DESTINATION .
    )



SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Application to visualize PLE-type data")
SET(CPACK_PACKAGE_VENDOR "Tomasz Kazimierczuk")
SET(CPACK_RESOURCE_FILE_LICENSE "${pleview_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${PLEVIEW_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PLEVIEW_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "pleview-${PLEVIEW_VERSION}")



IF(UNIX AND NOT APPLE)   
    SET(CPACK_GENERATOR "RPM")
    SET(CPACK_STRIP_FILES "pleview")
    SET(CPACK_PACKAGE_ICON "${pleview_SOURCE_DIR}/img/icon-installer.gif")
ENDIF(UNIX AND NOT APPLE)


GET_TARGET_PROPERTY(QtCore_dll         Qt5::Core         LOCATION)
GET_TARGET_PROPERTY(QtGui_dll          Qt5::Gui          LOCATION)
GET_TARGET_PROPERTY(QtWidgets_dll      Qt5::Widgets      LOCATION)
GET_TARGET_PROPERTY(QtSvg_dll          Qt5::Svg          LOCATION)
GET_TARGET_PROPERTY(QtPrintSupport_dll Qt5::PrintSupport LOCATION)


 
IF(WIN32)
     INSTALL(FILES
         ${QtCore_dll} ${QtGui_dll} ${QtWidgets_dll} ${QtSvg_dll} ${QtPrintSupport_dll}
         ${_qt5Core_install_prefix}/bin/icudt53.dll
         ${_qt5Core_install_prefix}/bin/icuin53.dll
         ${_qt5Core_install_prefix}/bin/icuuc53.dll
         DESTINATION .
     )


     GET_TARGET_PROPERTY(QtWindows_plugin   Qt5::QWindowsIntegrationPlugin LOCATION)
     INSTALL(FILES
          ${QtWindows_plugin}
          DESTINATION platforms
     )

     GET_TARGET_PROPERTY(QSvgIcon_plugin   Qt5::QSvgIconPlugin LOCATION)
     INSTALL(FILES
          ${QSvgIcon_plugin}
          DESTINATION iconengines
     )


    # Our path for customization of install procedure
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install" ${CMAKE_MODULE_PATH})

    SET(CPACK_GENERATOR "NSIS")
    SET(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    SET(CPACK_PACKAGE_EXECUTABLES "pleview;PLEview-${PLEVIEW_VERSION}")
    SET(CPACK_PACKAGE_ICON "${pleview_SOURCE_DIR}/img\\\\icon-installer.gif")
    SET (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/img/icon-installer.ico")
    SET (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/img/icon-uninstaller.ico")
    #SET (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/NSIS\\\\qpid-install-banner.bmp")
    
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "pleview.exe")
    SET(CPACK_NSIS_DISPLAY_NAME "PLEview ${PLEVIEW_VERSION}")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.fuw.edu.pl/~tkaz/pleview")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.fuw.edu.pl/~tkaz/pleview")
    SET(CPACK_NSIS_CONTACT "Tomasz.Kazimierczuk@fuw.edu.pl")
    SET(CPACK_NSIS_MODIFY_PATH ON)
ENDIF(WIN32)

INCLUDE(CPack)
