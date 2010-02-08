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
		MESSAGE("AudioEngine is OSX only, it won't be compiled on this platform")
	ENDIF ( APPLE )
ENDIF ( USE_OPENAL )
