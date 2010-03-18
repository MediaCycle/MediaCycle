IF ( USE_OSG )
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	ADD_LIBRARY(mediacycle-osg STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(mediacycle-osg mediacycle)

	TARGET_LINK_LIBRARIES(mediacycle-osg mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg osg osgDB osgGA osgViewer osgText)
	TARGET_LINK_LIBRARIES(mediacycle-osg OpenThreads)
	IF ( MINGW )
		TARGET_LINK_LIBRARIES(mediacycle-osg ${OSGPLUGINS_STATIC_LDFLAGS})#${OSGPLUGINS_LDFLAGS})
	ENDIF ( MINGW )

	TARGET_LINK_LIBRARIES(mediacycle-osg ${Boost_SYSTEM_LIBRARY})

	# TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)
	# TODO test on win32 : does it find opencv libs ?
	#IF ( USE_OPENCV )
	#	IF ( UNIX )
	#		TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENCV_CV_LIBRARY} ${OPENCV_CXCORE_LIBRARY} ${OPENCV_HIGHGUI_LIBRARY}) #CF compatibility with OpenCV frameworks in OSX
	#	ELSE ( UNIX )
			TARGET_LINK_LIBRARIES(mediacycle-osg cv cvaux highgui)
	#	ENDIF ( UNIX )
	#ENDIF ( USE_OPENCV )

	#everything is already set in the first CMakeLists.txt (but the linkage of course) :
	# -DUSE_OPENGL
	# include_directories
	IF ( WIN32 )
		TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENGL_LIBRARIES} ${FREEGLUT_LIBRARIES})
	ELSE ( WIN32 )
		TARGET_LINK_LIBRARIES(mediacycle-osg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ENDIF ( WIN32 )
ENDIF ( USE_OSG )
