SET(TARGET_NAME "MediaWorkflows")
SET(ICON_NAME "${TARGET_NAME}")
IF (SUPPORT_AUDIO AND (USE_QT4 OR USE_QT5) AND USE_OSG AND USE_OSC )

        SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC ON)
        SET(CONNECT_OSC ON)

        SET(MC_PLUGINS_STATIC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine-stk-sounddesign;audio-features-yaafe;audio-osg;audio-reader-sndfile;io-osc;audio-thumbnailer-waveform-svg;visualisation;visualisation-advanced;visualisation-tagging")
        SET(OSG_PLUGINS "osgdb_svg")
        SET(SETTING_INSTALLATION OFF)
        SET(CLUSTERS_POSITIONS "MediaCycle t_Sne")
        SET(DESCRIPTION "MediaWorkflows is an application developed as proof of concept in collaboration with sound design studio Dame Blanche, allowing exploratory recomposition with digital audio effects from audio files organized by similarity.")

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
