IF(USE_OSC)
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-io-osc ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-io-osc mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-io-osc mediacycle ${LIBLO_LIBRARY})
	SET_TARGET_PROPERTIES("mediacycle-io-osc" PROPERTIES FOLDER "${FOLDERNAME}")
	
	MESSAGE("[X] mediacycle-io-osc")
ELSE()
	MESSAGE("[ ] mediacycle-io-osc")
ENDIF()
