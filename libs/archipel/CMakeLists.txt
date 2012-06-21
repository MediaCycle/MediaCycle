IF ( CLUCENE_FOUND AND SUPPORT_ARCHIPEL )
#	MESSAGE("Building archipel ")
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(archipel STATIC ${SRC} ${HDR})
	IF(TINYXML_FOUND)
		TARGET_LINK_LIBRARIES(archipel ${TINYXML_LIBRARY})
	ELSE()
		ADD_DEPENDENCIES(archipel tinyxml)
		TARGET_LINK_LIBRARIES(archipel tinyxml)
	ENDIF()
	TARGET_LINK_LIBRARIES(archipel ${CLUCENE_LIBRARIES})

	TARGET_LINK_LIBRARIES(archipel ${Boost_LOCALE_LIBRARY})
	
	IF(Java_JAVAC_EXECUTABLE AND Java_JAR_EXECUTABLE)
		add_custom_command(
    		TARGET archipel
   			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} -classpath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -sourcepath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/htmlparser/*.java;${CMAKE_SOURCE_DIR}/3rdparty/nanoxml/net/n3/nanoxml/*.java
			#COMMAND ${Java_JAR_EXECUTABLE} cvfe ${CMAKE_CURRENT_BINARY_DIR}/archipelHtmlToXml.jar htmlparser/HtmlParser -C ${CMAKE_CURRENT_BINARY_DIR} htmlparser/*.class net/n3/nanoxml/*.class #CF this works only with Java 1.6+, so not OSX 10.5 (?) and older Ubuntus
			COMMAND ${Java_JAR_EXECUTABLE} cfvm ${CMAKE_CURRENT_BINARY_DIR}/archipelHtmlToXml.jar ${CMAKE_CURRENT_SOURCE_DIR}/Manifest.mf -C ${CMAKE_CURRENT_BINARY_DIR} htmlparser/*.class net/n3/nanoxml/*.class
		)
	ENDIF()
	MESSAGE("[X] archipel")
ELSE()
	MESSAGE("[ ] archipel")
ENDIF()
