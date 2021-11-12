IF(SUPPORT_AUDIO AND USE_VAMP)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(vamp-simple-host ${SRC} ${HDR})

	ADD_DEPENDENCIES(vamp-simple-host vamp-simple-hostsdk)

	TARGET_LINK_LIBRARIES(vamp-simple-host vamp-hostsdk vamp-sdk ${SNDFILE_LIBRARY}  stdc++)
	IF(NOT MINGW)
		TARGET_LINK_LIBRARIES(vamp-simple-host dl)
	ELSE()
		TARGET_LINK_LIBRARIES(vamp-simple-host FLAC vorbisenc vorbis m ogg wsock32 ws2_32)
	ENDIF()
	MESSAGE("[X] vamp-simple-host")
ELSE()
	MESSAGE("[ ] vamp-simple-host")
ENDIF()

