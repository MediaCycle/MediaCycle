IF(SUPPORT_AUDIO AND USE_VAMP)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/vamp-plugin-sdk)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-vamp)

	ADD_EXECUTABLE(vamp-simple-host ${SRC} ${HDR})

	ADD_DEPENDENCIES(vamp-simple-host vamp-hostsdk)

	TARGET_LINK_LIBRARIES(vamp-simple-host vamp-hostsdk vamp-sdk ${SNDFILE_LIBRARIES}  stdc++)
	IF(NOT MINGW)
		TARGET_LINK_LIBRARIES(vamp-simple-host dl)
	ELSE()
		TARGET_LINK_LIBRARIES(vamp-simple-host wsock32 ws2_32)
	ENDIF()
	MESSAGE("[X] vamp-simple-host")
ELSE()
	MESSAGE("[ ] vamp-simple-host")
ENDIF()

