SET(TARGET_NAME "ListeningRoom")
SET(ICON_NAME "${TARGET_NAME}")
IF ( SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND USE_BREAKPAD)

	SET(WITH_OSC ON)
	SET(CONNECT_OSC ON)
	SET(FULLSCREEN ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine-stk-mashta;audio-osg;audio-reader-sndfile;audio-thumbnailer-waveform-svg;io-osc;visualisation")
        SET(CLUSTERS_METHOD "MediaCycle KMeans")
        SET(CLUSTERS_POSITIONS "MediaCycle t_SNE")
	SET(OSG_PLUGINS "osgdb_ffmpeg")
	SET(SETTING_INSTALLATION ON)
	SET(MAIN_CLASS "ACAudioCycleListeningRoom")
        SET(DESCRIPTION "The Listening Room is an interactive room for collaborative music creation, using a sound map organized by similarity of collections inspired by paintings of Magritte. This application version is customized for installations (fullscreen, loops over libraries with a timer).")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} dep)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} dep)

	IF(APPLE)
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/ListeningRoomLibraries.xml" DESTINATION ${TARGET_NAME}.app/Contents/MacOS COMPONENT ${TARGET_NAME})
	ELSEIF(WIN32)
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/ListeningRoomLibraries.xml" DESTINATION . COMPONENT ${TARGET_NAME})
	ELSE()
		INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/ListeningRoomLibraries.xml" DESTINATION share COMPONENT ${TARGET_NAME})
	ENDIF()
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG)
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()


    SET(FOLDERNAME "tests")
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
