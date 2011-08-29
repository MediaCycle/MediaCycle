SET(TARGET_NAME "textcycle-osg-qt")
IF ( SUPPORT_TEXT AND USE_QT4 AND USE_OSG AND USE_OSC )

	IF ( USE_APPLE_MULTITOUCH )
		INCLUDE_DIRECTORIES( ../../libs/io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	INCLUDE_DIRECTORIES( ../multimediacycle-osg-qt ../../libs/gui-osg-qt)

	SET(multimediacycle-osg-qt_SRCS main.cpp ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.cpp)
	SET(multimediacycle-osg-qt_MOC_HDRS ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.h)
	SET(multimediacycle-osg-qt_UIS ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.ui)

	SET(settings_SRCS ../multimediacycle-osg-qt/settings.cpp ../multimediacycle-osg-qt/pluginsTreeItem.cpp ../multimediacycle-osg-qt/pluginsTreeModel.cpp)
	SET(settings_MOC_HDRS ../multimediacycle-osg-qt/settings.h ../multimediacycle-osg-qt/pluginsTreeModel.h) # pluginsTreeItem.h pluginsTreeModel.h)
	SET(settings_UIS ../multimediacycle-osg-qt/settings.ui)
	SET(settings_HS ../multimediacycle-osg-qt/pluginsTreeItem.h)

	# Generates ui_ACMultiMediaCycleOsgQt.h  and ui_settings.h
	QT4_WRAP_UI(multimediacycle-osg-qt_UIS_H ${multimediacycle-osg-qt_UIS})
	QT4_WRAP_UI(settings_UIS_H ${settings_UIS})

	# Generates moc_ACMultiMediaCycleOsgQt.cxx and moc_settings.cxx
	QT4_WRAP_CPP(multimediacycle-osg-qt_MOC_SRCS ${multimediacycle-osg-qt_MOC_HDRS})
	QT4_WRAP_CPP(settings_MOC_SRCS ${settings_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)

	IF ( WIN32 )
		ADD_EXECUTABLE(textcycle-osg-qt WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
	ELSE ()
		IF ( APPLE )
			ADD_EXECUTABLE(textcycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ELSE ()
			ADD_EXECUTABLE(textcycle-osg-qt ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ENDIF()
	ENDIF()

	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(textcycle-osg-qt multimediacycle-osg-qt)
	ADD_DEPENDENCIES(textcycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(textcycle-osg-qt mc_text mc_text_sparse mc_SparseVisualisation)
	IF ( USE_APPLE_MULTITOUCH )
		ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	#IF ( APPLE )
	#	set_target_properties(textcycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(textcycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-io-osc)
	#TARGET_LINK_LIBRARIES(textcycle-osg-qt ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(textcycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(textcycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(textcycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation")
		IF ( USE_APPLE_MULTITOUCH )
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()