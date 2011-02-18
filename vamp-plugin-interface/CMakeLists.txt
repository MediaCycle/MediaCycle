IF(SUPPORT_AUDIO AND USE_VAMP)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(vamp-plugin-interface STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(vamp-plugin-interface vamp-simple-hostsdk)
	INCLUDE_DIRECTORIES(. ../lib/vamp-plugin-sdk ../armadillo)

	TARGET_LINK_LIBRARIES(vamp-plugin-interface vamp-hostsdk vamp-sdk armadillo-utils ${SNDFILE_LIBRARY})
	MESSAGE("[X] vamp-plugin-interface")
ELSE()
	MESSAGE("[ ] vamp-plugin-interface")
ENDIF()


