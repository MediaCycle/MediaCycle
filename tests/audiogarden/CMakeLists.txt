IF(SUPPORT_AUDIO AND USE_AUDIOGARDEN)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(audiogarden-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(audiogarden-test mediacycle armadillo-utils audiofeatures audiogarden-synthesis mc_segmentation mc_audio)

	TARGET_LINK_LIBRARIES(audiogarden-test mediacycle audiofeatures audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	MESSAGE("[X] audiogarden")
ELSE()
	MESSAGE("[ ] audiogarden")
ENDIF()
