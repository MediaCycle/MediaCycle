
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)


	ADD_EXECUTABLE(armadillo-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(armadillo-test armadillo-utils armadillo-visualisation)
	TARGET_LINK_LIBRARIES(armadillo-test mediacycle armadillo-utils armadillo-visualisation ${ARMADILLO_LIBRARIES})


	SET_TARGET_PROPERTIES("armadillo-test" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] armadillo")
