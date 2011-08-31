IF(CLUCENE_FOUND)
#	MESSAGE("Building textTool ")
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(textTool STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(textTool tinyxml archipel)

	TARGET_LINK_LIBRARIES(textTool ${CLUCENE_LIBRARIES} archipel)
	MESSAGE("[X] TextTool")
ELSE()
	MESSAGE("[ ] TextTool (clucene not found)")
ENDIF()
