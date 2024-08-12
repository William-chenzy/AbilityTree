message("***************build InstallTool***************")
project(${INSTALLTOOL} VERSION ${INSTALLTOOL_VERSION})

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno -ffast-math")
else()
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /level='requireAdministrator' /uiAccess='false'")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories( ${DEPENDENSY_PATH}/miniz )

#file
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
	source_group("Resources" ".* \\.(ui|qrc|rc)")
endif()

add_executable(${INSTALLTOOL}	WIN32
	${UI_SRCS}
	${UI_HDRS}
	${ALLHEARDERS}
	${QRC_FILES}
	${RC}
	${QRC}
	${VERSION_RC}
	)

target_link_libraries(${INSTALLTOOL} PRIVATE
				Qt5::Core
				Qt5::Gui
				Qt5::Widgets
				Qt5EventDispatcherSupport
				Qt5FontDatabaseSupport
				qtmain
				Qt5ThemeSupport
				Qt5WindowsUIAutomationSupport
				qtlibpng
				qtfreetype
				qtharfbuzz
				qtpcre2
				qwindows
    )
