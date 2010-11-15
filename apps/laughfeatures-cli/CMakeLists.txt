IF (USE_LAUGHFEAT)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(laughfeatures-cli ${SRC} ${HDR})

	ADD_DEPENDENCIES(laughfeatures-cli armadillo-utils audiofeatures)
	ADD_DEPENDENCIES(laughfeatures-cli mc_audio mc_segmentation)
	INCLUDE_DIRECTORIES(. ../../armadillo ../../audio)

	TARGET_LINK_LIBRARIES(laughfeatures-cli audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	INSTALL(TARGETS laughfeatures-cli RUNTIME DESTINATION bin)

ENDIF (USE_LAUGHFEAT)
