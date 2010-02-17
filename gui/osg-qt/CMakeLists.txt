IF ( USE_QT4 )
	MESSAGE("gui/osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../osg)

	ADD_LIBRARY(mediacycle-osg-qt STATIC ${SRC} ${HDR})
	ADD_DEFINITIONS(-DQT_DLL)
	ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(mediacycle-osg-qt mediacycle-osg)

	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt OpenThreads)
	#TARGET_LINK_LIBRARIES(mediacycle-osg-qt TiCommon TiIo TiOsc)
	#TARGET_LINK_LIBRARIES(mediacycle-osg-qt TiCommon TiRendering)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	#IF ( APPLE )
	#	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	#ENDIF ( APPLE )

	IF ( OPENGL_FOUND AND GLUT_FOUND)
		TARGET_LINK_LIBRARIES(mediacycle-osg-qt ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ELSE ( OPENGL_FOUND AND GLUT_FOUND )
		MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
	ENDIF ( OPENGL_FOUND AND GLUT_FOUND ) 

ELSE ( USE_QT4 )
	MESSAGE("Qt4 disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
