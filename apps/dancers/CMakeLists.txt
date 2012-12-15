IF(SUPPORT_VIDEO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB MC_SRC *.cpp *.c)
	FILE(GLOB MC_HDR *.hpp *.h)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/video-core)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/video-features)

	ADD_EXECUTABLE(mediacycle-dancers ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-dancers mediacycle mc_visualisation mc_video)

	TARGET_LINK_LIBRARIES(mediacycle-dancers mediacycle)

	SET_TARGET_PROPERTIES("mediacycle-dancers" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] dancers")
ELSE()
	MESSAGE("[ ] dancers")
ENDIF()

