SET(PLUGIN_NAME "library-json")
#IF() #only requires boost which is a mediacycle dependency
	INCLUDE (${CMAKE_SOURCE_DIR}/cmake/AddMediaCyclePlugin.cmake)
        ADD_MC_PLUGIN_LIBRARY(${PLUGIN_NAME})
        TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${Boost_LOCALE_LIBRARY})
	MESSAGE("[X] ${TARGET_NAME}")
#ELSE()
#	MESSAGE("[ ] ${TARGET_NAME}")
#ENDIF()
