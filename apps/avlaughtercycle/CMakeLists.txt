IF(SUPPORT_AUDIO AND USE_AUDIOFEAT AND BUILD_CLIS)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(mediacycle-avlc ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-avlc mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-avlc mediacycle)

	MESSAGE("[X] avlaughtercycle")
ELSE()
	MESSAGE("[ ] avlaughtercycle")
ENDIF()
