message("***************build Wireshark***************")
project(${WIRESHARK} VERSION ${WIRESHARK_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories( ${DEPENDENSY_PATH}/WpdPack/Include )
if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -pthread -lpcap")
	link_directories( ${DEPENDENSY_PATH}/WpdPack/Lib/ )
else()
	link_directories( ${DEPENDENSY_PATH}/WpdPack/Lib/x64 )
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

#file
file(GLOB_RECURSE RC *.rc )
file(GLOB_RECURSE ALLHEARDERS *.h )
file(GLOB_RECURSE UI_SRCS *.cpp)
file(GLOB_RECURSE UI_FILES *.ui )
qt5_wrap_ui(UI_HDRS ${UI_FILES})

if(MSVC)
	source_group("Generated Files" FILES ${UI_FILES} ${QRC_FILES})
	source_group("Resources" ".*\\.(ui|qrc|rc)")
endif()


add_executable(${WIRESHARK} WIN32
	${UI_SRCS}
	${UI_HDRS}
	${ALLHEARDERS}
	${QRC_FILES}
	${RC}
	${VERSION_RC}
	)

if(WIN32)
	target_link_libraries( ${WIRESHARK} packet wpcap )
else()
	target_link_libraries( ${WIRESHARK} pcap )
endif()

target_link_libraries(${WIRESHARK}
    Qt5::Core
    Qt5::Gui 
    Qt5::Widgets    
    Qt5::PrintSupport
    Qt5::SerialPort
    Qt5::Network
    ${OpenCV_LIBS}
    )

install(TARGETS ${PROJECT_NAME} DESTINATION  ${INSTALL_PLAT_PATH})
