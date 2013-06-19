SET(TARGET_NAME "KnownItemSearchCycle")
IF ( SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

        SET(WITH_SEGMENTATION OFF)
        SET(WITH_OSC ON)

        SET(MC_PLUGINS_STATIC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "usability-kis;audio-engine;audio-features-yaafe;audio-reader-sndfile;audio-thumbnailer-waveform-svg;audio-osg;visualisation;media-thumbnailer-feature-mapper;io-osc")#visualisation-advanced
        SET(OSG_PLUGINS "osgdb_svg") #required by audio-thumbnailer-waveform-svg
        SET(DESCRIPTION "KnownItemSearchCycle is a stripped-down version of AudioCycle, aimed at organizing audio files by similarity, tailored for usability testing.")

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
