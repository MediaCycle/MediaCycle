SET(PLUGIN_NAME "pcapreprocess")
#IF(NOT(WIN32 OR MINGW))
        INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
        ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${TARGET_NAME}")
#ELSE()
#	MESSAGE("[ ] ${TARGET_NAME}")
#ENDIF()
