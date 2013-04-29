SET(TARGET_NAME "MashtaCycle")
SET(ICON_NAME "${TARGET_NAME}")
IF (SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC )

        SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC ON)

        #IF(MINGW AND CMAKE_TOOLCHAIN_FILE)
                SET(MC_PLUGINS_STATIC ON)
        #ENDIF()

	SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
        #SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-osg;audio-reader-sndfile;audio-segmentation;file;io-osc;io-3m-multitouch-screen;pcapreprocess;segmentation;visualisation")
        SET(MC_PLUGINS "audio-engine-stk;audio-osg;audio-reader-sndfile;io-osc;audio-thumbnailer-waveform;file;visualisation")#io-3m-multitouch-screen;
        SET(OSG_PLUGINS "osgdb_svg")
        #SET(SETTING_INSTALLATION ON)
        SET(DESCRIPTION "Mash'taCycle is an application tailored for Gauthier Keyaerts aka The Very Mash'ta, allowing audio collages from audio files organized by similarity.")

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
	
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()