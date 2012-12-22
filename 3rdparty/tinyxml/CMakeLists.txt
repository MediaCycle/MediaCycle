IF(NOT TINYXML_FOUND)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(tinyxml ${SRC} ${HDR})

	#gsl - uncomment if needed
	TARGET_LINK_LIBRARIES(tinyxml stdc++)

	#INSTALL(TARGETS tinyxml LIBRARY DESTINATION lib)
	#INSTALL(FILES ${HDR} DESTINATION include/tinyxml)
	SET_TARGET_PROPERTIES("tinyxml" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] TinyXML")
ELSE()
	MESSAGE("[ ] TinyXML (using installed version)")
ENDIF()