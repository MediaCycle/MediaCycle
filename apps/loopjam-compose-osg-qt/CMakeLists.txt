SET(TARGET_NAME "LoopCycle")
IF ( BUILD_LOOPJAM_APP AND SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) AND USE_BREAKPAD )

	SET(USE_AUDIO ON)
	SET(USE_IMAGE OFF)
	SET(USE_VIDEO OFF)
	SET(USE_3DMODEL OFF)
	SET(USE_TEXT OFF)
	SET(USE_ARCHIPEL OFF)
	SET(WITH_QT4 ON)
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "LoopJam is an interactive dancefloor for collaborative music creation, using a sound map organized by similarity. This is application version is for composing and authoring loop libraries for LoopJam.")

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
