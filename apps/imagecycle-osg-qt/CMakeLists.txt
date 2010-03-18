IF ( USE_QT4 )
	MESSAGE("apps/imagecycle-osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(imagecycle-osg-qt_SRCS main.cpp ACImageCycleOsgQt.cpp)
	SET(imagecycle-osg-qt_MOC_HDRS ACImageCycleOsgQt.h)
	SET(imagecycle-osg-qt_UIS ACImageCycleOsgQt.ui)

	# Generates ui_ACImageCycleOsgQt.h
	QT4_WRAP_UI(imagecycle-osg-qt_UIS_H ${imagecycle-osg-qt_UIS})

	# Generates moc_ACImageCycleOsgQt.cxx
	QT4_WRAP_CPP(imagecycle-osg-qt_MOC_SRCS ${imagecycle-osg-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( APPLE )
		ADD_EXECUTABLE(imagecycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${imagecycle-osg-qt_SRCS} ${imagecycle-osg-qt_UIS_H} ${imagecycle-osg-qt_MOC_SRCS})
	ELSE ( APPLE )
		ADD_EXECUTABLE(imagecycle-osg-qt WIN32 ${imagecycle-osg-qt_SRCS} ${imagecycle-osg-qt_UIS_H} ${imagecycle-osg-qt_MOC_SRCS})
	ENDIF ( APPLE )

	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle-osg-qt)

	#IF ( APPLE )
	#	set_target_properties(imagecycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt OpenThreads)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )

	IF ( WIN32 )
		TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${OPENGL_LIBRARIES} ${FREEGLUT_LIBRARIES})
	ELSE ( WIN32 )
		TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ENDIF ( WIN32 ) 

ELSE ( USE_QT4 )
	MESSAGE("imagecycle-osg-qt disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
