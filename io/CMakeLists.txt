IF ( USE_OSC )
        MESSAGE("io")
	ADD_SUBDIRECTORY(osc)
ENDIF ( USE_OSC )

IF ( USE_APPLE_MULTITOUCH )
        MESSAGE("applemultitouch")
	ADD_SUBDIRECTORY(applemultitouch)
ENDIF ()

