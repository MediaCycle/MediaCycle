IF(APPLE AND USE_OSG AND USE_COCOA)
	FILE(GLOB SRC *.m *.mm)
	FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-osg-cocoa ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle-osg)

	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle ${ARMADILLO_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa ${OPENGL_LIBRARIES} ${COREDATA_LIBRARY} ${COREVIDEO_LIBRARY} ${FOUNDATION_LIBRARY} ${APPKIT_LIBRARY})
	MESSAGE("[X] mediacycle-osg-cocoa")
ELSE()
	MESSAGE("[ ] mediacycle-osg-cocoa")
ENDIF()
