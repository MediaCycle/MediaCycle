# Apple multitouch trackpad support
IF ( APPLE AND USE_APPLE_MULTITOUCH )
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-io-applemultitouch ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-io-applemultitouch mediacycle)
    TARGET_LINK_LIBRARIES(mediacycle-io-applemultitouch mediacycle ${APPLE_MULTITOUCH_LIBRARY} ${COREFOUNDATION_LIBRARY})
    SET_TARGET_PROPERTIES("mediacycle-io-applemultitouch" PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] mediacycle-io-applemultitouch")
ELSE()
	MESSAGE("[ ] mediacycle-io-applemultitouch")
ENDIF()
