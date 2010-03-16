IF ( APPLE AND USE_COCOA)
	FILE(GLOB SRC *.m *.mm)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../osg)

	ADD_LIBRARY(mediacycle-osg-cocoa STATIC ${SRC} ${HDR})

	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-cocoa mediacycle-osg)

	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa TiCommon TiRendering)
	TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa osg osgDB osgGA osgViewer)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework Foundation")

	IF ( OPENGL_FOUND AND GLUT_FOUND)
		TARGET_LINK_LIBRARIES(mediacycle-osg-cocoa ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ELSE ( OPENGL_FOUND AND GLUT_FOUND )
		MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
	ENDIF ( OPENGL_FOUND AND GLUT_FOUND )
ENDIF ( APPLE AND USE_COCOA)
								   
