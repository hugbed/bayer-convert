cmake_minimum_required(VERSION 3.8)
project(bayer_convert)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES
        include/*.h
        src/*.cpp)

include_directories(include)

# Next lines needed for building all Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 5 COMPONENTS Core Widgets REQUIRED )

set ( MOC_HEADERS
  include/ui/mainwindow.h
)

set ( UIS
  ui/mainwindow.ui
)

QT5_WRAP_UI( UI_HEADERS ${UIS} )

add_executable(${PROJECT_NAME} ${SRC_FILES} ${UI_HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets ${OpenCV_LIBS} stdc++fs -pthread)
