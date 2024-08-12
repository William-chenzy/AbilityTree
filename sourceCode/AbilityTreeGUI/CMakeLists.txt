message("***************build AbilityTreeViewer***************")
project(${GUI_ABILITY_TREE} VERSION ${GUI_ABILITY_TREE_VER})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -ffast-math -pthread")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

#lib
set(LIBS ${LIBS} ${LX_LIBRARY_NAME})

file(GLOB RC *.rc )
file(GLOB ALLHPP *.hpp )
file(GLOB UI_SRCS *.cpp)
file(GLOB UI_FILES *.ui )
file(GLOB ALLHEARDERS *.h )
file(GLOB QRC ../../include/*.qrc )
qt5_wrap_ui(UI_HDRS ${UI_FILES})

if(MSVC)
	source_group("Generated Files" FILES ${UI_FILES} ${QRC_FILES})
	source_group("Resources" "../../include/* \\.(ui|qrc|rc)")
	source_group("Resources" ".*\\.(ui|qrc|rc)")
endif()

add_executable(${GUI_ABILITY_TREE} WIN32
	${QM_FILES}
	${UI_SRCS}
	${UI_HDRS}
	${UI_FILES}
	${ALLHEARDERS}
	${ALLHPP}
	${QRC_FILES}
	${RC}
	${QRC}
	${VERSION_RC}
	)

target_link_libraries(${GUI_ABILITY_TREE}
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	Qt5::PrintSupport
	Qt5::Network
	${OpenCV_LIBS}
	)

