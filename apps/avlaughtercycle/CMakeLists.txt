IF(SUPPORT_AUDIO AND USE_AUDIOFEAT AND BUILD_CLIS)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-core)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/audio-features-laughter)

	ADD_EXECUTABLE(mediacycle-avlc ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-avlc mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-avlc mediacycle)

	SET_TARGET_PROPERTIES("mediacycle-avlc" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] avlaughtercycle")
ELSE()
	MESSAGE("[ ] avlaughtercycle")
ENDIF()
