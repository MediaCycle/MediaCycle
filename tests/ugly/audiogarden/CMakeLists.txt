IF(SUPPORT_AUDIO AND USE_AUDIOGARDEN AND USE_AUDIOFEAT)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-core)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-engine)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audiogarden-synthesis)

	ADD_EXECUTABLE(audiogarden-test ${SRC} ${HDR})

        ADD_DEPENDENCIES(audiogarden-test mediacycle armadillo-utils mediacycle-audio-features mediacycle-audiogarden-synthesis mc_segmentation mc_audio-segmentation)

        TARGET_LINK_LIBRARIES(audiogarden-test mediacycle mediacycle-audio-features mediacycle-audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARIES})

	MESSAGE("[X] audiogarden")
ELSE()
	MESSAGE("[ ] audiogarden")
ENDIF()
