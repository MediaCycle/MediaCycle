SET(TARGET_NAME "LoopCycle")
IF ( BUILD_LOOPJAM_APP AND SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) AND USE_BREAKPAD )

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine;audio-features-acid;audio-features-yaafe;audio-osg;audio-reader-sndfile;file;io-osc;pcapreprocess;visualisation")
        SET(CLUSTERS_METHOD "MediaCycle KMeans")
        SET(CLUSTERS_POSITIONS "AudioGarden Flower")
	SET(OSG_PLUGINS "")
        SET(DESCRIPTION "LoopJam is an interactive dancefloor for collaborative music creation, using a sound map organized by similarity. This is application version is for composing and authoring loop libraries for LoopJam.")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} ...)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} ...)
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()