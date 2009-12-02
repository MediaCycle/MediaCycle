MESSAGE("gui/osg-qt")
project (AudioCycleQtOsg) 

FILE(GLOB SRC *.cpp)
FILE(GLOB HDR *.h)

INCLUDE_DIRECTORIES(. ../osg)

#IF ( APPLE )
#	SET(APP_TYPE MACOSX_BUNDLE)
#ENDIF ( APPLE )

IF ( USE_QT4 )
	SET(MediaCycleQtOsg_SRCS main.cpp ACMainWindowQT.cpp ACOsgBrowserViewQT.cpp)
	SET(MediaCycleQtOsg_MOC_HDRS ACMainWindowQT.h)
	SET(MediaCycleQtOsg_UIS ACMainWindowQT.ui)

	# Generates ui_ACMainWindowQT.h
	QT4_WRAP_UI(MediaCycleQtOsg_UIS_H ${MediaCycleQtOsg_UIS})

	# Generates moc_ACMainWindowQT.cxx
	QT4_WRAP_CPP(MediaCycleQtOsg_MOC_SRCS ${MediaCycleQtOsg_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	#Now add these generated files to the ADD_EXECUTABLE
	# step. If this is NOT done, then the ui_*.h files
	# will not be generated

	ADD_DEFINITIONS(-DQT_DLL)
	ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)


	#IF ( APPLE )
	#	ADD_EXECUTABLE(MediaCycleQtOsg ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${MediaCycleQtOsg_SRCS} ${MediaCycleQtOsg_UIS_H} ${MediaCycleQtOsg_MOC_SRCS})
	#ELSE ( APPLE )
		ADD_EXECUTABLE(MediaCycleQtOsg ${MediaCycleQtOsg_SRCS} ${MediaCycleQtOsg_UIS_H} ${MediaCycleQtOsg_MOC_SRCS})
	#ENDIF ( APPLE )

	ADD_DEPENDENCIES(MediaCycleQtOsg mediacycle)
	ADD_DEPENDENCIES(MediaCycleQtOsg mediacycle-osg)

	#IF ( APPLE )
	#	set_target_properties(MediaCycleQtOsg PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(MediaCycleQtOsg mediacycle)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg mediacycle-osg)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg OpenThreads)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg TiCommon TiIo TiOsc TiRendering)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg TiCommon TiRendering)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg boost_system)
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(MediaCycleQtOsg ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	#IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	#ENDIF ( APPLE )

	IF ( OPENGL_FOUND AND GLUT_FOUND)
		TARGET_LINK_LIBRARIES(MediaCycleQtOsg ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ELSE ( OPENGL_FOUND AND GLUT_FOUND )
		MESSAGE(FATAL_ERROR "OpenGL/Glut requested but not found.")
	ENDIF ( OPENGL_FOUND AND GLUT_FOUND ) 

ELSE ( USE_QT4 )
	MESSAGE("Qt4 disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
