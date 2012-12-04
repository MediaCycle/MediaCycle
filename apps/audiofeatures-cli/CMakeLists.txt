SET(TARGET_NAME "audiofeatures-cli")
IF (SUPPORT_AUDIO AND USE_AUDIOFEAT AND BUILD_CLIS)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-core)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features)

	ADD_EXECUTABLE(audiofeatures-cli ${SRC} ${HDR})

	ADD_DEPENDENCIES(audiofeatures-cli mediacycle armadillo-utils mediacycle-audio-features)
	ADD_DEPENDENCIES(audiofeatures-cli mc_audio mc_segmentation)

	TARGET_LINK_LIBRARIES(audiofeatures-cli mediacycle mediacycle-audio-features ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
