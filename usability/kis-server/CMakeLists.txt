# Forked from Klaus Schoeffmann VBS server for MMM 2013 VBS
SET(TARGET_NAME "kis-server")
	add_custom_target (
		${TARGET_NAME} ALL
	)
	IF(Java_JAVAC_EXECUTABLE AND Java_JAR_EXECUTABLE)
		add_custom_command(
    		TARGET ${TARGET_NAME}
   			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} -classpath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -sourcepath ${CMAKE_CURRENT_SOURCE_DIR}:${CMAKE_SOURCE_DIR}/3rdparty/nanoxml -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.java;${CMAKE_SOURCE_DIR}/3rdparty/nanoxml/net/n3/nanoxml/*.java
			COMMAND ${Java_JAR_EXECUTABLE} cfvm ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.jar ${CMAKE_CURRENT_SOURCE_DIR}/Manifest.mf -C ${CMAKE_CURRENT_BINARY_DIR} *.class net/n3/nanoxml/*.class
		)
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${TARGET_NAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
