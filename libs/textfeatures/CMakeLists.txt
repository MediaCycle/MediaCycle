IF ( SUPPORT_TEXT  )
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(textfeatures STATIC ${SRC} ${HDR})

	TARGET_LINK_LIBRARIES(textfeatures textTool ${CLUCENE_LIBRARIES} )
	ADD_DEPENDENCIES(textfeatures textTool mediacycle)

	MESSAGE("[X] textfeatures")
ELSE ( SUPPORT_TEXT)
	MESSAGE("[ ] textfeatures")
ENDIF ( SUPPORT_TEXT )