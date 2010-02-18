FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

IF ( USE_OPENAL )
	IF ( APPLE )
		MESSAGE("AudioEngine enabled")
		INCLUDE_DIRECTORIES(.)

		ADD_LIBRARY(audioengine STATIC ${SRC} ${HDR})
		ADD_DEPENDENCIES(audioengine mediacycle)

		TARGET_LINK_LIBRARIES(audioengine mediacycle)
	ELSE ( APPLE )
		IF ( USE_ALUT )
			MESSAGE("AudioEngine enabled")
			INCLUDE_DIRECTORIES(.)

			ADD_LIBRARY(audioengine STATIC ${SRC} ${HDR})
			ADD_DEPENDENCIES(audioengine mediacycle)

			TARGET_LINK_LIBRARIES(audioengine mediacycle)
		ELSE ( USE_ALUT )
			MESSAGE("AudioEngine disabled (ALUT required)")
		ENDIF ( USE_ALUT )		
	ENDIF ( APPLE )
ENDIF ( USE_OPENAL )
