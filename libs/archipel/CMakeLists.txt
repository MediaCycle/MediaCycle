IF ( CLUCENE_FOUND  )
#	MESSAGE("Building archipel ")
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(archipel STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(archipel tinyxml)

	TARGET_LINK_LIBRARIES(archipel ${CLUCENE_LIBRARIES})

	IF(Java_JAVAC_EXECUTABLE AND Java_JAR_EXECUTABLE)
		add_custom_command(
    		TARGET archipel
   			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} -classpath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdParty/nanoxml -sourcepath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdParty/nanoxml -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/htmlparser/*.java ;${CMAKE_SOURCE_DIR}/3rdParty/nanoxml/net/n3/nanoxml/*.java
			COMMAND ${Java_JAR_EXECUTABLE} cvfe ${CMAKE_CURRENT_BINARY_DIR}/archipelHtmlToXml.jar htmlparser/HtmlParser -C ${CMAKE_CURRENT_BINARY_DIR} htmlparser/*.class net/n3/nanoxml/*.class
		)
	ENDIF()
	MESSAGE("[X] archipel")
ELSE()
	MESSAGE("[ ] archipel")
ENDIF()