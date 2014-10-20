SET(TARGET_NAME "AudioCycle")
IF ( BUILD_AUDIOCYCLE_APP AND SUPPORT_AUDIO AND (USE_QT4 OR USE_QT5) AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

        SET(WITH_SEGMENTATION ON)
	SET(WITH_OSC ON)

        SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-features-yaafe;audio-engine-stk-sounddesign;audio-osg;audio-reader-sndfile;audio-segmentation;io-osc;pcapreprocess;segmentation;visualisation")
	IF(USE_DEBUG)
                LIST(APPEND MC_PLUGINS "visualisation-advanced;visualisation-tagging")
	ENDIF()
        IF(MLPACK_FOUND)
            IF(USE_DEBUG)
                LIST(APPEND MC_PLUGINS "visualisation-advanced-mlpack")
            ENDIF()
            LIST(APPEND MC_PLUGINS "visualisation-mlpack")
        ENDIF()
        IF(APPLE)
            LIST(APPEND MC_PLUGINS "visualisation-divvy")
        ENDIF()
        IF(NEWMAT_FOUND)
            LIST(APPEND MC_PLUGINS "visualisation-lineartimenerv;visualisation-dredviznerv")
        ENDIF()
        IF(USE_FALCON AND LIBNIFALCON_FOUND AND LIBUSB_FOUND)
                LIST(APPEND MC_PLUGINS "io-novint-falcon")
        ENDIF()
	LIST(APPEND MC_PLUGINS "visualisation-s-hull")

	LIST(APPEND MC_PLUGINS "audio-thumbnailer-waveform-svg;audio-thumbnailer-features-svg")
	#SET(OSG_PLUGINS "osgdb_svg") #required by audio-thumbnailer-waveform-svg

        SET(DESCRIPTION "AudioCycle allows to organize audio files by similarity.")
        #SET(FILTERING "Proximity Grid")
        SET(CLUSTERS_POSITIONS "MediaCycle Import Grid")

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
