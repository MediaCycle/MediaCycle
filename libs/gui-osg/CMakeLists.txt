IF(USE_OSG)
	FILE(GLOB mediacycle-osg_SRC *.cpp)
	FILE(GLOB mediacycle-osg_HDR *.h)

	ADD_LIBRARY(mediacycle-osg ${mediacycle-osg_SRC} ${mediacycle-osg_HDR})
	ADD_DEPENDENCIES(mediacycle-osg mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg mediacycle)

        #TARGET_LINK_LIBRARIES(mediacycle-osg osgdb_svg)
        IF(CMAKE_TOOLCHAIN_FILE AND MINGW)
            ADD_DEPENDENCIES(mediacycle-osg osgdb_freetype)
            TARGET_LINK_LIBRARIES(mediacycle-osg osgdb_freetype)
        ENDIF()

	IF(MINGW)#OR APPLE_IOS_DEV OR APPLE_IOS_SIM)
		TARGET_LINK_LIBRARIES(mediacycle-osg ${OSGPLUGINS_STATIC_LDFLAGS})#${OSGPLUGINS_LDFLAGS})
	ENDIF(MINGW)#OR APPLE_IOS_DEV OR APPLE_IOS_SIM)
        TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENSCENEGRAPH_LIBRARIES} ${OPENGL_LIBRARIES} ${Boost_SYSTEM_LIBRARY})

	SET_TARGET_PROPERTIES("mediacycle-osg" PROPERTIES FOLDER "${FOLDERNAME}")

	MESSAGE("[X] mediacycle-osg")
ELSE()
	MESSAGE("[ ] mediacycle-osg")
ENDIF()
