cmake_minimum_required(VERSION 3.5)
project(AbilityTree)

#define version
set(GUI_ABILITY_TREE_VER 1.0.0)

string(TIMESTAMP COMPILE_TIME %Y%m%d)
add_definitions(-DCOMPILE_TIME="${COMPILE_TIME}")

#define name
set(GUI_ABILITY_TREE LxCameraApi)

#set path
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(DEPENDENSY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty)
set(INSTALL_RELEASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/release)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV QUIET)
if(${OPENCV_FOUND})
message("OPENCV_FOUND " ${OpenCV_INCLUDE_DIRS})
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_directories(${OpenCV_DIR})
    add_definitions(-DHAS_OPENCV)
endif()

option(BUILD_VIEWER "" ON)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport SerialPort Network QUIET)

if(BUILD_VIEWER)
add_subdirectory(AbilityTreeGUI)
endif()
