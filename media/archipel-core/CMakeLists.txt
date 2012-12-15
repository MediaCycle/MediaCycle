SET(MEDIA_TYPE "archipel")
SET(LIBRARY_NAME "mediacycle-${MEDIA_TYPE}-core")
IF(SUPPORT_ARCHIPEL)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(../audio-core ../image-core ../text-core)
	ADD_LIBRARY(${LIBRARY_NAME} ${SRC} ${HDR})
	ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle-audio-core mediacycle-image-core mediacycle-text-core)
	TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-audio-core mediacycle-image-core mediacycle-text-core)
	IF(TINYXML_FOUND)
		TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${TINYXML_LIBRARY})
	ELSE()
		ADD_DEPENDENCIES(${LIBRARY_NAME} tinyxml)
		TARGET_LINK_LIBRARIES(${LIBRARY_NAME} tinyxml)
	ENDIF()

	IF(Java_JAVAC_EXECUTABLE AND Java_JAR_EXECUTABLE)
		add_custom_command(
    		TARGET ${LIBRARY_NAME}
   			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} -classpath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -sourcepath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/htmlparser/*.java;${CMAKE_SOURCE_DIR}/3rdparty/nanoxml/net/n3/nanoxml/*.java
			#COMMAND ${Java_JAR_EXECUTABLE} cvfe ${CMAKE_CURRENT_BINARY_DIR}/archipelHtmlToXml.jar htmlparser/HtmlParser -C ${CMAKE_CURRENT_BINARY_DIR} htmlparser/*.class net/n3/nanoxml/*.class #CF this works only with Java 1.6+, so not OSX 10.5 (?) and older Ubuntus
			COMMAND ${Java_JAR_EXECUTABLE} cfvm ${CMAKE_CURRENT_BINARY_DIR}/archipelHtmlToXml.jar ${CMAKE_CURRENT_SOURCE_DIR}/Manifest.mf -C ${CMAKE_CURRENT_BINARY_DIR} htmlparser/*.class net/n3/nanoxml/*.class
		)
	ENDIF()
	SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${LIBRARY_NAME}")
ELSE()
	MESSAGE("[ ] ${LIBRARY_NAME}")
ENDIF()
