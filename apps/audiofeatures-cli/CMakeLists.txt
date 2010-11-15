IF (USE_AUDIOFEAT)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(audiofeatures-cli ${SRC} ${HDR})

	ADD_DEPENDENCIES(audiofeatures-cli armadillo-utils audiofeatures)
	ADD_DEPENDENCIES(audiofeatures-cli mc_audio mc_segmentation)
	INCLUDE_DIRECTORIES(. ../../armadillo ../../audio)

	TARGET_LINK_LIBRARIES(audiofeatures-cli audiofeatures ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	INSTALL(TARGETS audiofeatures-cli RUNTIME DESTINATION bin)

ENDIF (USE_AUDIOFEAT)
