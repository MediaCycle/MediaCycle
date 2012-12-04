SET(LIBRARY_NAME "mediacycle-segmentation")
#IF()
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(${LIBRARY_NAME} ${SRC} ${HDR})
	TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle armadillo-utils)

	MESSAGE("[X] ${LIBRARY_NAME}")
#ELSE()
#	MESSAGE("[ ] ${LIBRARY_NAME}")
#ENDIF()
