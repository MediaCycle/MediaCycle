SET(TARGET_NAME "navimedcycle-osg-qt")
IF ( SUPPORT_NAVIMED AND SUPPORT_MULTIMEDIA AND SUPPORT_IMAGE AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG )

	SET(USE_AUDIO OFF)
	SET(USE_IMAGE ON)
	SET(USE_VIDEO OFF)
	SET(USE_3DMODEL OFF)
	SET(USE_TEXT ON)
	SET(USE_ARCHIPEL OFF)
	SET(USE_NAVIMED ON)
	SET(WITH_QT4 ON)
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "NavimedCycle is an attempt to complement the Navimed database from Polymedis (Belgium) with the MediaCycle organization of media files by similarity.")

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