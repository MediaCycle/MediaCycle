	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(audiogarden-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(audiogarden-test armadillo-utils audiofeatures audiogarden mc_segmentation mc_audio)
	INCLUDE_DIRECTORIES(. ../../armadillo ../../audio ../../ ../../fftooura ../../audiogarden)

	TARGET_LINK_LIBRARIES(audiogarden-test audiofeatures audiogarden ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} ${SDIF_LIBRARY})

	INSTALL(TARGETS audiogarden-test RUNTIME DESTINATION bin)
