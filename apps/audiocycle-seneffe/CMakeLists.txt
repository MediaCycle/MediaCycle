SET(TARGET_NAME "audiocycle-seneffe")
IF ( SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

	IF ( USE_APPLE_MULTITOUCH )
		INCLUDE_DIRECTORIES( ../../libs/io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	INCLUDE_DIRECTORIES( ../multimediacycle-osg-qt ../../libs/gui-osg-qt)

	SET(multimediacycle-osg-qt_SRCS main.cpp ACMultiMediaCycleSeneffe.cpp ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.cpp)
	SET(multimediacycle-osg-qt_MOC_HDRS ACMultiMediaCycleSeneffe.h ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.h)
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
		ADD_EXECUTABLE(audiocycle-seneffe WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
	ELSE ()
		IF ( APPLE )
			ADD_EXECUTABLE(audiocycle-seneffe ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ELSE ()
			ADD_EXECUTABLE(audiocycle-seneffe ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ENDIF()
	ENDIF()

	ADD_DEPENDENCIES(audiocycle-seneffe mediacycle)
	ADD_DEPENDENCIES(audiocycle-seneffe mediacycle-io-osc)
	ADD_DEPENDENCIES(audiocycle-seneffe oscpack)
	ADD_DEPENDENCIES(audiocycle-seneffe mediacycle-osg)
	ADD_DEPENDENCIES(audiocycle-seneffe mediacycle-osg-qt)
	ADD_DEPENDENCIES(audiocycle-seneffe multimediacycle-osg-qt)
	ADD_DEPENDENCIES(audiocycle-seneffe audioengine)
	ADD_DEPENDENCIES(audiocycle-seneffe mc_visualisation)
	ADD_DEPENDENCIES(audiocycle-seneffe mc_audio)
	ADD_DEPENDENCIES(audiocycle-seneffe mc_segmentation)
	IF ( USE_APPLE_MULTITOUCH )
		ADD_DEPENDENCIES(audiocycle-seneffe mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	#IF ( APPLE )
	#	set_target_properties(audiocycle-seneffe PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(audiocycle-seneffe mediacycle)
	TARGET_LINK_LIBRARIES(audiocycle-seneffe mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiocycle-seneffe mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiocycle-seneffe mediacycle-io-osc)
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe oscpack)
	TARGET_LINK_LIBRARIES(audiocycle-seneffe audioengine)
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${OPENAL_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-seneffe ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH )
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(audiocycle-seneffe mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
