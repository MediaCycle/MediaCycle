SET(PLUGIN_NAME "greta")
SET(TARGET_NAME "${PLUGIN_PREFIX}${PLUGIN_NAME}")
IF ( USE_PSYCLONE )
    FILE(GLOB SRC *.cpp *.c)
    FILE(GLOB HDR *.hpp *.h)

# NB: linux only for now, should work with OSX as well, won't work with windows (what is the install path on win32 ?)
# this supposes that cpp AIR plug is installed. (see http://www.mindmakers.org/projects/Psyclone/)
# copy AIR/AIR.a in /usr/local/lib/libAIR.a
# copy CoreLibrary/CoreLibrary.a in /usr/local/lib/libCoreLibrary.a
# copy AIR/include/*.h in /usr/local/include/AIR
# copy CoreLibrary/include/*.h & CoreLibrary/include/thirdparty/*.h in /usr/local/include/CoreLibrary/

    INCLUDE_DIRECTORIES( "/usr/local/include/AIR" )
    INCLUDE_DIRECTORIES( "/usr/local/include/CoreLibrary" )

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D Linux")

    ADD_LIBRARY(${TARGET_NAME} SHARED ${SRC} ${HDR})
    ADD_DEPENDENCIES(${TARGET_NAME} mediacycle)

    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PREFIX ${PLUGIN_PREFIX})
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})
    #SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES SUFFIX ${PLUGIN_SUFFIX})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} AIR)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} CoreLibrary)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} pthread dl)

    #INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
