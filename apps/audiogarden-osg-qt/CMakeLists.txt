SET(TARGET_NAME "AudioGarden")
SET(ICON_NAME "${TARGET_NAME}")
IF (SUPPORT_AUDIO AND USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

	SET(WITH_SEGMENTATION ON)
	SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-reader-sndfile;audio-osg;audio-segmentation;audiogarden;file;pcapreprocess;segmentation;visualisation")
        SET(CLUSTERS_METHOD "MediaCycle KMeans")
        SET(CLUSTERS_POSITIONS "AudioGarden Flower")
        SET(OSG_PLUGINS "")
        SET(DESCRIPTION "AudioGarden is an attempt to provide a usable tool for composite audio creation.")

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
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/cmake/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
