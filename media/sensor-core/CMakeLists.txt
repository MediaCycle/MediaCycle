SET(MEDIA_TYPE "sensor")
SET(LIBRARY_NAME "mediacycle-${MEDIA_TYPE}-core")
IF(SUPPORT_SENSOR)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(${LIBRARY_NAME} ${SRC} ${HDR})
	ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle)
	TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle)
	#INSTALL(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib/${LIBRARY_NAME})
	SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${LIBRARY_NAME}")
ELSE()
	MESSAGE("[ ] ${LIBRARY_NAME}")
ENDIF()
