SET(TARGET_NAME "AudioMetroHTML5")
IF ( BUILD_AUDIOCYCLE_APP AND SUPPORT_AUDIO AND (USE_QT4 OR USE_QT5) AND MLPACK_FOUND)

        SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC OFF)

        SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-features-yaafe;audio-reader-sndfile;pcapreprocess;visualisation;visualisation-mapping;visualisation-mlpack;audio-thumbnailer-features-svg;library-json")

        SET(LOCAL_LIBRARY_FILENAME "library.json")
        SET(LOCAL_LIBRARY_WRITER_PLUGIN_NAME "JSON export")
        SET(WEBPAGE_RELATIVE_SOURCEPATH "3rdparty/audiometro-html5/index.html")

        SET(DESCRIPTION "AudioMetro allows to organize sound files by similarity.")
        #SET(FILTERING "Proximity Grid")
        SET(CLUSTERS_POSITIONS "MediaCycle Import Grid")

	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCycleWebkitQtApp.cmake)
        ADD_MC_WEBKIT_QT_EXECUTABLE(${TARGET_NAME})

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
