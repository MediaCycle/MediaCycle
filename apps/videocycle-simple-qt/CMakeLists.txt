IF ( USE_QT4 )
	MESSAGE("apps/videocycle-simple-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt)
	#INCLUDE_DIRECTORIES(../../plugins/video)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(videocycle-simple-qt_SRCS main.cpp MyGUI.cpp)
	SET(videocycle-simple-qt_MOC_HDRS MyGUI.h)
	SET(videocycle-simple-qt_UIS VideoCycle.ui)

	# Generates ui_VideoCycle.h
	QT4_WRAP_UI(videocycle-simple-qt_UIS_H ${videocycle-simple-qt_UIS})

	# Generates moc_MyGUI.cxx 
	QT4_WRAP_CPP(videocycle-simple-qt_MOC_SRCS ${videocycle-simple-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(videocycle-simple-qt WIN32 ${videocycle-simple-qt_SRCS} ${videocycle-simple-qt_UIS_H} ${videocycle-simple-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(videocycle-simple-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${videocycle-simple-qt_SRCS} ${videocycle-simple-qt_UIS_H} ${videocycle-simple-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(videocycle-simple-qt ${videocycle-simple-qt_SRCS} ${videocycle-simple-qt_UIS_H} ${videocycle-simple-qt_MOC_SRCS} )
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(videocycle-simple-qt mc_video)
	ADD_DEPENDENCIES(videocycle-simple-qt mediacycle)
	ADD_DEPENDENCIES(videocycle-simple-qt mediacycle-osg)
	ADD_DEPENDENCIES(videocycle-simple-qt mediacycle-osg-qt)

	#IF ( APPLE )
	#	set_target_properties(videocycle-simple-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(videocycle-simple-qt mc_video)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt mediacycle)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt OpenThreads)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(videocycle-simple-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(videocycle-simple-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(videocycle-simple-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )
	TARGET_LINK_LIBRARIES(videocycle-simple-qt ${OPENGL_LIBRARIES})

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )

ELSE ( USE_QT4 )
	MESSAGE("videocycle-simple-qt disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
