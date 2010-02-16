MESSAGE("io/osc")
IF ( USE_OSC )

FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

INCLUDE_DIRECTORIES(. ../../)

	ADD_SUBDIRECTORY(oscpack)
	INCLUDE_DIRECTORIES(oscpack)

	ADD_LIBRARY(mediacycle-io-osc STATIC ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-io-osc mediacycle)
	ADD_DEPENDENCIES(mediacycle-io-osc oscpack)
	ADD_DEPENDENCIES(mediacycle-io-osc audioengine)
	TARGET_LINK_LIBRARIES(mediacycle-io-osc mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-io-osc oscpack)

ENDIF ( USE_OSC )
