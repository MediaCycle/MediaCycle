	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(armadillo-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(armadillo-test mediacycle armadillo-utils)

	TARGET_LINK_LIBRARIES(armadillo-test mediacycle)
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(armadillo-test ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})
	ENDIF()
	TARGET_LINK_LIBRARIES(armadillo-test atlas lapack blas armadillo-utils)
	MESSAGE("[X] armadillo")
