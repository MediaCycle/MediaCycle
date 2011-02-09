IF ( USE_QT4 )
	MESSAGE("apps/multimediacycle-osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt ../../tinyxml)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(multimediacycle-osg-qt_SRCS main.cpp ACMultiMediaCycleOsgQt.cpp)
	SET(multimediacycle-osg-qt_MOC_HDRS ACMultiMediaCycleOsgQt.h)
	SET(multimediacycle-osg-qt_UIS ACMultiMediaCycleOsgQt.ui)

	SET(settings_SRCS settings.cpp)
	SET(settings_MOC_HDRS settings.h)
	SET(settings_UIS settings.ui)

	# Generates ui_ACMultiMediaCycleOsgQt.h  and ui_settings.h
	QT4_WRAP_UI(multimediacycle-osg-qt_UIS_H ${multimediacycle-osg-qt_UIS})
	QT4_WRAP_UI(settings_UIS_H ${settings_UIS})

	# Generates moc_ACMultiMediaCycleOsgQt.cxx and moc_settings.cxx
	QT4_WRAP_CPP(multimediacycle-osg-qt_MOC_SRCS ${multimediacycle-osg-qt_MOC_HDRS})
	QT4_WRAP_CPP(settings_MOC_SRCS ${settings_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(multimediacycle-osg-qt WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(multimediacycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(multimediacycle-osg-qt ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${multimediacycle-osg-qt_MOC_HDRS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg-qt)

	#IF ( APPLE )
	#	set_target_properties(multimediacycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt OpenThreads)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt osg osgDB osgGA osgViewer)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
ELSE ( USE_QT4 )
	MESSAGE("multimediacycle-osg-qt disabled - not compiling Qt GUI.")
ENDIF ( USE_QT4 )
