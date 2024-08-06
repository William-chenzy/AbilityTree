cmake_minimum_required(VERSION 3.5)
project(AbilityTree)

#define version
set(GUI_ABILITY_TREE_VER 1.0.0)
set(NET_CONFIG_TOOL_VERSION 1.1.0)
set(WIRESHARK_VERSION 1.1.0)
set(COMMUNICATION_TOOL_VERSION 1.1.0)
set(QUICKMACRO_VERSION 1.1.0)
set(PEFILEANALYAZER_VERSION 1.1.0)
set(PHOTOSTOOL_VERSION 1.0.0)
set(THREADINJECTION_VERSION 1.0.0)


string(TIMESTAMP COMPILE_TIME %Y%m%d)
add_definitions(-DCOMPILE_TIME="${COMPILE_TIME}")
add_definitions(-DWIN32_LEAN_AND_MEAN)

#define name 
set(GUI_ABILITY_TREE AbilityTreeViewer)
set(NET_CONFIG_TOOL NetConfigTool)
set(WIRESHARK Wireshark)
set(COMMUNICATION_TOOL CommunicationTool)
set(QUICKMACRO QuickMacro)
set(PEFILEANALYAZER PeFileAnalyzer)
set(PHOTOSTOOL PhotosTool)
set(THREADINJECTION ThreadInjection)


#set path
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin/)
set(DEPENDENSY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty)
set(INSTALL_RELEASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/release)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV QUIET)
if(${OPENCV_FOUND})
	message("OPENCV_FOUND " ${OpenCV_INCLUDE_DIRS})
    include_directories(${OpenCV_INCLUDE_DIRS})
    link_directories(${OpenCV_DIR})
    add_definitions(-DHAS_OPENCV)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)


if (WIN32)
option(BUILD_QuickMacro "" ON)
option(BUILD_NetConfigTool "" ON)
option(BUILD_PeFileAnalyzer "" ON)
option(BUILD_ThreadInjection "" ON)
else()
endif()

option(BUILD_VIEWER "" ON)
option(BUILD_Wireshark "" ON)
option(BUILD_CommunicationTool "" ON)
option(BUILD_PhotosTool "" ON)


find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport SerialPort Network QUIET)

if(NOT ${QT5_FOUND})
	message("Qt5 not found!")
	return()
endif()

if(BUILD_VIEWER)
add_subdirectory(AbilityTreeGUI)
endif()

if(BUILD_Wireshark)
add_subdirectory(Wireshark)
endif()

if(BUILD_NetConfigTool)
add_subdirectory(NetConfigTool)
endif()

if(BUILD_CommunicationTool)
add_subdirectory(CommunicationTool)
endif()

if(BUILD_QuickMacro)
add_subdirectory(QuickMacro)
endif()

if(BUILD_PeFileAnalyzer)
add_subdirectory(PeFileAnalyzer)
endif()

if(BUILD_PhotosTool)
add_subdirectory(PhotosTool)
endif()

if(BUILD_ThreadInjection)
add_subdirectory(ThreadInjection)
endif()

