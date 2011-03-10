IF(SUPPORT_AUDIO AND USE_AUDIOFEAT)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(mediacycle-avlc ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-avlc mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-avlc mediacycle)

	INSTALL(TARGETS mediacycle-avlc RUNTIME DESTINATION bin)
	MESSAGE("[X] avlaughtercycle")
ELSE()
	MESSAGE("[ ] avlaughtercycle")
ENDIF()
