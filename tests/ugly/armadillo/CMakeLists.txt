
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)


	ADD_EXECUTABLE(armadillo-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(armadillo-test armadillo-utils armadillo-isomap)
	TARGET_LINK_LIBRARIES(armadillo-test mediacycle armadillo-utils armadillo-isomap ${ARMADILLO_LIBRARIES})


	MESSAGE("[X] armadillo")
