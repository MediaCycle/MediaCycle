IF ( USE_OSC )

FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-io-osc ${SRC} ${HDR})

    INCLUDE_DIRECTORIES (../../core/)
	ADD_DEPENDENCIES(mediacycle-io-osc mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-io-osc mediacycle)
	
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(mediacycle-io-osc audioengine)
		TARGET_LINK_LIBRARIES(mediacycle-io-osc audioengine)
	ENDIF()
	
    TARGET_LINK_LIBRARIES(mediacycle-io-osc ${LIBLO_LIBRARY})
	
	MESSAGE("[X] mediacycle-io-osc")
ELSE()
	MESSAGE("[ ] mediacycle-io-osc")
ENDIF()
