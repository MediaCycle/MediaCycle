SET(PLUGIN_NAME "library-csv")
#IF() #only requires boost which is a mediacycle dependency
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
        ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
	MESSAGE("[X] ${TARGET_NAME}")
#ELSE()
#	MESSAGE("[ ] ${TARGET_NAME}")
#ENDIF()
