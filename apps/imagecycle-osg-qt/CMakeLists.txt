SET(TARGET_NAME "ImageCycle")
IF ( BUILD_IMAGECYCLE_APP AND SUPPORT_IMAGE AND USE_QT4 AND USE_OSG AND USE_OSC )

	SET(MEDIA_TYPE "MEDIA_TYPE_IMAGE")
	SET(MC_PLUGINS "file;image;image-osg;pcapreprocess;visualisation")
	SET(OSG_PLUGINS "")
        SET(DESCRIPTION "ImageCycle allows to organize image files by similarity.")

	INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreateDefaultConfig.cmake)
	CREATE_DEFAULT_CONFIG(${TARGET_NAME})

	INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} )
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} )
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
