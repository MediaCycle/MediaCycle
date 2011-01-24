IF (USE_LAUGHFEAT)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(laughfeatures-cli ${SRC} ${HDR})

	INCLUDE_DIRECTORIES(. ../../core ../../armadillo ../../audio ../../plugins/segmentation)

	ADD_DEPENDENCIES(laughfeatures-cli mediacycle armadillo-utils audiofeatures mc_audio mc_segmentation)

	TARGET_LINK_LIBRARIES(laughfeatures-cli mediacycle audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	INSTALL(TARGETS laughfeatures-cli RUNTIME DESTINATION bin)

ENDIF (USE_LAUGHFEAT)
