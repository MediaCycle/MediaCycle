	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(armadillo-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(armadillo-test armadillo-utils)
	INCLUDE_DIRECTORIES(. ../../armadillo ../../ ../../fftooura)

	TARGET_LINK_LIBRARIES(armadillo-test ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} ${SDIF_LIBRARY} atlas lapack blas armadillo-utils)

	INSTALL(TARGETS armadillo-test RUNTIME DESTINATION bin)
