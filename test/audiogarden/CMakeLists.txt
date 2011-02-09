	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(audiogarden-test ${SRC} ${HDR})

	INCLUDE_DIRECTORIES(. ../../core ../../armadillo ../../audio ../../ ../../fftooura ../../audiogarden ../../tinyxml)

	ADD_DEPENDENCIES(audiogarden-test mediacycle armadillo-utils audiofeatures audiogarden mc_segmentation mc_audio)

	TARGET_LINK_LIBRARIES(audiogarden-test mediacycle audiofeatures audiogarden ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	INSTALL(TARGETS audiogarden-test RUNTIME DESTINATION bin)
