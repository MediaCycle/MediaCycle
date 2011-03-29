SET(TARGET_NAME "multimediacycle-osg-qt")
IF (BUILD_CLIS)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_EXECUTABLE(mediacycle-cli ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-cli mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-cli mediacycle tinyxml)

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
