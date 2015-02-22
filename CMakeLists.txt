PROJECT(pleview)

CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

SET(CMAKE_CXX_FLAGS "-std=c++11")

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


#IF(WIN32)
#SET(BOOST_ROOT "C:\\Program Files\\boost\\boost_1_42")
#ENDIF(WIN32)
#FIND_PACKAGE(Boost REQUIRED)
#INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )


INCLUDE_DIRECTORIES(
    ${pleview_SOURCE_DIR}/src
    ${pleview_SOURCE_DIR}/qwt-tk/src
    ${pleview_SOURCE_DIR}/qwt-tk/src-tk
    ${pleview_SOURCE_DIR}/muparser/include
    ${pleview_SOURCE_DIR}/zlib/
)

ADD_SUBDIRECTORY(qwt-tk)
ADD_SUBDIRECTORY(muparser)
ADD_SUBDIRECTORY(zlib)


SET(PLEVIEW_VERSION_MAJOR "0")
SET(PLEVIEW_VERSION_MINOR "18")
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
)
QT5_WRAP_CPP(MOC_SRCS ${MOC_HDRS})

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
)

QT5_ADD_RESOURCES( RC_SRCS  ${pleview_SOURCE_DIR}/img/resources.qrc)


ADD_EXECUTABLE(pleview  ${Pleview_SRCS} ${MOC_SRCS} ${RC_SRCS} ${pleview_SOURCE_DIR}/img/pleview.rc)
TARGET_LINK_LIBRARIES(pleview Qt5::Widgets Qt5::Svg Qt5::PrintSupport Qt5::Concurrent qwt-tk muparser zlib)

IF(WIN32)
TARGET_LINK_LIBRARIES(pleview ${QT_QTMAIN_LIBRARY})
SET_PROPERTY(TARGET pleview PROPERTY ENABLE_EXPORTS 1)
ENDIF(WIN32)

#ADD_SUBDIRECTORY(src/plugins)


INSTALL(TARGETS pleview
        DESTINATION bin
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
    SET(CPACK_STRIP_FILES "bin/pleview")
    SET(CPACK_PACKAGE_ICON "${pleview_SOURCE_DIR}/img/icon-installer.gif")
ENDIF(UNIX AND NOT APPLE)



IF(WIN32)
     INSTALL(FILES
         "${QT_LIBRARY_DIR}/../bin/QtCore${QT_VERSION_MAJOR}.dll"
         "${QT_LIBRARY_DIR}/../bin/QtXml${QT_VERSION_MAJOR}.dll"
         "${QT_LIBRARY_DIR}/../bin/QtSvg${QT_VERSION_MAJOR}.dll"
		 "${QT_LIBRARY_DIR}/../bin/QtSvg${QT_VERSION_MAJOR}.dll"
         "${QT_LIBRARY_DIR}/../bin/QtGui${QT_VERSION_MAJOR}.dll"
         "${QT_LIBRARY_DIR}/../plugins/iconengines/qsvgicon${QT_VERSION_MAJOR}.dll"
         DESTINATION bin
     )
     INSTALL(FILES
         "${QT_LIBRARY_DIR}/../plugins/iconengines/qsvgicon${QT_VERSION_MAJOR}.dll"        
          DESTINATION bin/iconengines
     )


    # Our path for customization of install procedure
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/install" ${CMAKE_MODULE_PATH})

    SET(CPACK_GENERATOR "NSIS")
    SET(CPACK_PACKAGE_EXECUTABLES "pleview;PLEview-${PLEVIEW_VERSION}")
    SET(CPACK_PACKAGE_ICON "${pleview_SOURCE_DIR}/img\\\\icon-installer.gif")
    SET (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/img/icon-installer.ico")
    SET (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/img/icon-uninstaller.ico")
    #SET (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/packaging/NSIS\\\\qpid-install-banner.bmp")
    
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\pleview.exe")
    SET(CPACK_NSIS_DISPLAY_NAME "PLEview ${PLEVIEW_VERSION}")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.fuw.edu.pl/~tkaz/pleview")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.fuw.edu.pl/~tkaz/pleview")
    SET(CPACK_NSIS_CONTACT "Tomasz.Kazimierczuk@fuw.edu.pl")
    SET(CPACK_NSIS_MODIFY_PATH ON)
ENDIF(WIN32)

INCLUDE(CPack)
