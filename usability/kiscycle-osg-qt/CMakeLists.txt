SET(TARGET_NAME "KnownItemSearchCycle")
IF ( SUPPORT_AUDIO AND (USE_QT4 OR USE_QT5) AND USE_OSG AND MLPACK_FOUND AND APPLE)

        SET(WITH_SEGMENTATION OFF)
        SET(WITH_OSC ON)
        SET(FULLSCREEN ON)
        SET(SETTING_INSTALLATION OFF)
        SET(HIDE_INFORMATION ON)
        SET(FORGET_THUMBNAILS OFF)
        SET(MC_PLUGINS_STATIC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "usability-kis;audio-engine-stk-sounddesign;audio-features-yaafe;audio-reader-sndfile;audio-osg;audio-thumbnailer-waveform-svg;audio-thumbnailer-features-svg;visualisation;io-osc")#visualisation-advanced
	LIST(APPEND MC_PLUGINS "visualisation-mlpack")
	IF(USE_FALCON)
		LIST(APPEND MC_PLUGINS "io-novint-falcon")
	ENDIF()
        IF(APPLE AND USE_APPLE_MULTITOUCH)
                LIST(APPEND MC_PLUGINS "io-apple-trackpad-autoplay")
        ENDIF()
        #SET(CLUSTERS_METHOD "MediaCycle KMeans")
        #SET(CLUSTERS_POSITIONS "MediaCycle t_Sne")
        #SET(OSG_PLUGINS "osgdb_svg") #required by audio-thumbnailer-waveform-svg
        SET(MAIN_CLASS "ACAudioCycleKIS")
        SET(DESCRIPTION "KnownItemSearchCycle is a stripped-down version of AudioCycle, aimed at organizing audio files by similarity, tailored for usability testing.")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} )
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} )
	
        IF(APPLE)
                INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/KISLibraries.xml" DESTINATION ${TARGET_NAME}.app/Contents/MacOS COMPONENT ${TARGET_NAME})
        ELSEIF(WIN32)
                INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/KISLibraries.xml" DESTINATION . COMPONENT ${TARGET_NAME})
        ELSE()
                INSTALL(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/KISLibraries.xml" DESTINATION share COMPONENT ${TARGET_NAME})
        ENDIF()


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
