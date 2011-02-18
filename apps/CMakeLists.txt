#List the files in "apps" and if there are directories, add them to the list of (potential) test programs

MESSAGE("\nApplications:")

FILE(GLOB DIRLIST * )

FOREACH(APPDIR ${DIRLIST})
    IF(IS_DIRECTORY ${APPDIR})
	# AudioGarden
	GET_FILENAME_COMPONENT(APPNAME ${APPDIR} NAME)
	IF ("${APPDIR}" MATCHES "audiogarden-osg-qt")
		IF (USE_AUDIOGARDEN)
			ADD_SUBDIRECTORY(${APPDIR})
		ELSE ()
			MESSAGE("[ ] ${APPNAME}")
		ENDIF()
	ELSEIF ("${APPDIR}" MATCHES "audiocycle-osg-ipad")
		IF ( APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE )
			ADD_SUBDIRECTORY(${APPDIR})
		ELSE()
			MESSAGE("[ ] ${APPNAME}")
		ENDIF()
	ELSE ()
		IF ( APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE )
		ELSE()
        		ADD_SUBDIRECTORY(${APPDIR})
		ENDIF()

	ENDIF ()
    ENDIF()
ENDFOREACH()
