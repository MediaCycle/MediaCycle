IF ( USE_PSYCLONE )
    FILE(GLOB SRC *.cpp *.c)
    FILE(GLOB HDR *.hpp *.h)

    SET(PLUGIN_NAME "greta")

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

    ADD_LIBRARY(${PLUGIN_NAME} SHARED ${SRC} ${HDR})
    ADD_DEPENDENCIES(${PLUGIN_NAME} mediacycle)

    SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES PREFIX "mc_")
    #SET_TARGET_PROPERTIES(${PLUGIN_NAME} PROPERTIES SUFFIX ".mcp")
    TARGET_LINK_LIBRARIES(${PLUGIN_NAME} mediacycle)
    TARGET_LINK_LIBRARIES(${PLUGIN_NAME} AIR)
    TARGET_LINK_LIBRARIES(${PLUGIN_NAME} CoreLibrary)
    TARGET_LINK_LIBRARIES(${PLUGIN_NAME} pthread dl)

    INSTALL(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})
ENDIF ( USE_PSYCLONE )
