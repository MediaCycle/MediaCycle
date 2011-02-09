	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(armadillo-test ${SRC} ${HDR})

	INCLUDE_DIRECTORIES(. ../core ../../armadillo ../../ ../../fftooura ../../tinyxml)

	ADD_DEPENDENCIES(armadillo-test mediacycle armadillo-utils)

	TARGET_LINK_LIBRARIES(armadillo-test mediacycle ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY} atlas lapack blas armadillo-utils)
	INSTALL(TARGETS armadillo-test RUNTIME DESTINATION bin)
