SET(TARGET_NAME "LoopJam")
SET(ICON_NAME "${TARGET_NAME}")
IF ( BUILD_LOOPJAM_APP AND SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) AND USE_BREAKPAD)

	SET(WITH_OSC ON)
	SET(CONNECT_OSC ON)
	SET(FULLSCREEN ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine;audio-osg;audio-reader-sndfile;audio-thumbnailer-waveform;io-osc;visualisation")
        SET(CLUSTERS_METHOD "MediaCycle KMeans")
        SET(CLUSTERS_POSITIONS "AudioGarden Flower")
	SET(OSG_PLUGINS "osgdb_ffmpeg")
	SET(MAIN_CLASS "ACAudioCycleLoopJam")
        SET(DESCRIPTION "LoopJam is an interactive dancefloor for collaborative music creation, using a sound map organized by similarity. This application version is customized for installations (fullscreen, loops over libraries with a timer).")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} dep)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} dep)

	IF(APPLE)
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/LoopJamLibraries.xml" DESTINATION ${TARGET_NAME}.app/Contents/MacOS COMPONENT ${TARGET_NAME})
	ELSEIF(WIN32)
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/LoopJamLibraries.xml" DESTINATION . COMPONENT ${TARGET_NAME})
	ELSE()
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/LoopJamLibraries.xml" DESTINATION share COMPONENT ${TARGET_NAME})
	ENDIF()
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()


    SET(FOLDERNAME "tests")
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
