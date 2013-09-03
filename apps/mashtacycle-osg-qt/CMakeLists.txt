SET(TARGET_NAME "MashtaCycle")
SET(ICON_NAME "${TARGET_NAME}")
IF (SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC )

        SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC ON)
        SET(CONNECT_OSC ON)

        SET(MC_PLUGINS_STATIC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-osg;audio-reader-sndfile;io-osc;audio-thumbnailer-waveform-svg;visualisation")
	SET(OSG_PLUGINS "osgdb_svg")
        #SET(SETTING_INSTALLATION ON)
        SET(CLUSTERS_POSITIONS "MediaCycle t_Sne")
        SET(DESCRIPTION "MashtaCycle is an application tailored for Gauthier Keyaerts aka The Very Mash'ta, allowing the desktop preparation of artistic performances/installations with audio collages from audio files organized by similarity.")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} )
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} )
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG)
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
