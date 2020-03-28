SET(TARGET_NAME "laughfeatures-cli")
IF(SUPPORT_AUDIO AND USE_LAUGHFEAT AND USE_AUDIOFEAT AND BUILD_CLIS)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-core)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features-laughter)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/segmentation)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugins/audio-segmentation)

	ADD_EXECUTABLE(laughfeatures-cli ${SRC} ${HDR})

        ADD_DEPENDENCIES(laughfeatures-cli mediacycle mediacycle-audio-core mediacycle-audio-features mediacycle-audio-features-laughter mediacycle-segmentation) #mediacycle-audio-segmentation

        TARGET_LINK_LIBRARIES(laughfeatures-cli mediacycle mediacycle-audio-features ${FFTW3_LIBRARY} ${SNDFILE_LIBRARIES})
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
