IF ( APPLE AND USE_COCOA )
	FILE(GLOB SRC *.m *.mm)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../osg)

	ADD_LIBRARY(mediacycle-osg-cocoa STATIC ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle-osg)

	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa ${OPENGL_LIBRARIES})
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework Foundation")
ENDIF ( APPLE AND USE_COCOA )