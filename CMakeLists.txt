cmake_minimum_required(VERSION 3.7)
project(bayer_convert)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES
        include/*.h
        src/*.cpp)

include_directories(include)

# Next lines needed for building all Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 5 COMPONENTS Core Widgets REQUIRED )

set(MOC_HEADERS
  include/ui/mainwindow.h)

set(UIS
  ui/mainwindow.ui)

QT5_WRAP_UI( UI_HEADERS ${UIS} )

# install
if (APPLE)
    # bundle variables
    set(MACOSX_BUNDLE_INFO_STRING "BayerToRGB")
#    set(MACOSX_BUNDLE_ICON_FILE "TeraPlus.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "BayerToRGB")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "BayerRGB")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${CPACK_PACKAGE_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "IntRolab UdeS")
    set(EXECUTABLE_FLAGS MACOSX_BUNDLE)

    set(QT_MACDEPLOYQT ${Qt5_DIR}/../../../bin/macdeployqt)
    install(CODE "execute_process(COMMAND ${QT_MACDEPLOYQT} ${PROJECT_NAME}.app)")
endif(APPLE)

add_executable(${PROJECT_NAME} ${EXECUTABLE_FLAGS} ${SRC_FILES} ${UI_HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets ${OpenCV_LIBS} -pthread)
