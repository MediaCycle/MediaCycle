IF ( USE_OSG )
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-osg STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-osg mediacycle)

	#-lmediacycle -lTiCommon -lTiIo -lTiOsc -lTiRendering
	#-losg -losgDB -losgGA -losgViewer -lOpenThreads
	#-lboost_system -lcv -lcvaux -lhighgui
	TARGET_LINK_LIBRARIES(mediacycle-osg mediacycle)
	#TARGET_LINK_LIBRARIES(mediacycle-osg TiCommon TiIo TiOsc TiRendering) #CF: only necessary for osg-cocoa
	TARGET_LINK_LIBRARIES(mediacycle-osg osg osgDB osgGA osgViewer osgText)
	TARGET_LINK_LIBRARIES(mediacycle-osg OpenThreads)

	#boost_system is looked for in the main CMakeLists.txt
	IF ( Boost_SYSTEM_FOUND )
		TARGET_LINK_LIBRARIES(mediacycle-osg ${Boost_SYSTEM_LIBRARY})
	ELSE ( )
		MESSAGE(FATAL_ERROR "Boost system not found.")
	ENDIF ( )

	# TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)
	# TODO test on win32 : does it find opencv libs ?
	IF ( USE_OPENCV )
		IF ( UNIX )
			TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY}) #CF compatibility with OpenCV frameworks in OSX
		ELSE ( UNIX )
			TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)
		ENDIF ( UNIX )
	ENDIF ( USE_OPENCV )

	#everything is already set in the first CMakeLists.txt (but the linkage of course) :
	# -DUSE_OPENGL
	# include_directories
	IF ( OPENGL_FOUND AND GLUT_FOUND)
		TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ELSE ( OPENGL_FOUND AND GLUT_FOUND )
		MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
	ENDIF ( OPENGL_FOUND AND GLUT_FOUND )
ENDIF ( USE_OSG )
