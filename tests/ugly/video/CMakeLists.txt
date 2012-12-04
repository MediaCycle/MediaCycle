IF(SUPPORT_VIDEO)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	FILE(GLOB PLUGIN_DIRS ${CMAKE_SOURCE_DIR}/plugins/*)
	FOREACH(PLUGIN_DIR ${PLUGIN_DIRS})
   		IF(IS_DIRECTORY ${PLUGIN_DIR})
        		INCLUDE_DIRECTORIES(${PLUGIN_DIR})
    		ENDIF()
	ENDFOREACH()

        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/gnuplot_i)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/image-core)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/image-features)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/video-core)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/video-features)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/cvBlobsLib)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/cvgabor)

	ADD_EXECUTABLE(video-test ${SRC} ${HDR})

        ADD_DEPENDENCIES(video-test mediacycle ${PLUGIN_PREFIX}video gnuplot_i)

        TARGET_LINK_LIBRARIES(video-test mediacycle ${PLUGIN_PREFIX}video gnuplot_i)

	MESSAGE("[X] video")
ELSE()
	MESSAGE("[ ] video")
ENDIF()
