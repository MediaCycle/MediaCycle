IF ( CLUCENE_FOUND  )
#	MESSAGE("Building archipel ")
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(archipel STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(archipel tinyxml)

	TARGET_LINK_LIBRARIES(archipel ${CLUCENE_LIBRARIES})
	MESSAGE("[X] archipel")
ELSE()
	MESSAGE("[ ] archipel")
ENDIF()