SET(TARGET_NAME "TextCycle")
IF ( BUILD_TEXTCYCLE_APP AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG AND USE_OSC )

	SET(WITH_SEGMENTATION OFF)
	SET(WITH_OSC OFF)

	SET(MEDIA_TYPE "MEDIA_TYPE_TEXT")
	SET(MC_PLUGINS "file;pcapreprocess;text;text-osg;visualisation")
	SET(OSG_PLUGINS "osgdb_freetype")
        SET(DESCRIPTION "TextCycle allows to organize text files by similarity.")

	INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} É)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} É)
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()
	
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()