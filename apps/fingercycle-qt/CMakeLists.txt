SET(TARGET_NAME "fingercycle-qt")
IF(USE_QT4 AND USE_OSC)
	IF(QT_VERSION_MAJOR LESS 4 AND QT_VERSION_MINOR LESS 7)
		MESSAGE("[ ] ${TARGET_NAME} (Qt>=4.7 is required for multitouch support)")
	ELSE()
        	SET(MC_PACKAGE_DESCRIPTION_SUMMARY "FingerCycle allows to test multiple-pointer input of MediaCycle applications though OSC.")
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/AddQtApp.cmake)
		ADD_QT_EXECUTABLE(${TARGET_NAME})

		TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBLO_LIBRARY})

		MESSAGE("[X] ${TARGET_NAME}")

		# Packaging
		SET(PROGNAME "${TARGET_NAME}")
		IF(NOT USE_DEBUG AND (APPLE OR UNIX))
			INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
		ENDIF()
	ENDIF()
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
