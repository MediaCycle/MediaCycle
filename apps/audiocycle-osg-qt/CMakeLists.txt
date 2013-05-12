SET(TARGET_NAME "AudioCycle")
IF ( BUILD_AUDIOCYCLE_APP AND SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

        SET(WITH_SEGMENTATION ON)
	SET(WITH_OSC ON)

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
	IF(USE_DEBUG)
        	SET(MC_PLUGINS "audio-features-yaafe;audio-engine;audio-osg;audio-reader-sndfile;audio-segmentation;audio-thumbnailer-waveform;file;io-osc;pcapreprocess;segmentation;visualisation;visualisation-advanced")
        	SET(OSG_PLUGINS "osgdb_svg") #required by audio-thumbnailer-waveform
	ELSE()
        	SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-osg;audio-reader-sndfile;audio-segmentation;audio-thumbnailer-waveform;file;io-osc;segmentation;visualisation")
        	SET(OSG_PLUGINS "osgdb_svg") #required by audio-thumbnailer-waveform
	ENDIF()
        SET(DESCRIPTION "AudioCycle allows to organize audio files by similarity.")

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
