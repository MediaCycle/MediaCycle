SET(MEDIA_TYPE "image")
SET(LIBRARY_NAME "mediacycle-${MEDIA_TYPE}-features")
IF((SUPPORT_IMAGE OR SUPPORT_VIDEO) AND FFTW3_FOUND AND OpenCV_FOUND)
	INCLUDE_DIRECTORIES(
		${CMAKE_SOURCE_DIR}/3rdparty/cvBlobsLib
		${CMAKE_SOURCE_DIR}/3rdparty/cvgabor
		${CMAKE_SOURCE_DIR}/media/image-core
	)

	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(${LIBRARY_NAME} ${SRC} ${HDR})

	ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle-${MEDIA_TYPE}-core)
	TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-${MEDIA_TYPE}-core cvBlobsLib cvgabor)
	SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] ${LIBRARY_NAME}")
ELSE()
	MESSAGE("[ ] ${LIBRARY_NAME}")
ENDIF()
