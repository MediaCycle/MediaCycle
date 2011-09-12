SET(TARGET_NAME "audiocycle-loopjam")
IF ( SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

	IF ( USE_APPLE_MULTITOUCH )
		INCLUDE_DIRECTORIES( ../../libs/io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	INCLUDE_DIRECTORIES( ../multimediacycle-osg-qt ../../libs/gui-osg-qt)

	SET(multimediacycle-osg-qt_SRCS main.cpp ACAudioCycleLoopJam.cpp ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.cpp)
	SET(multimediacycle-osg-qt_MOC_HDRS ACAudioCycleLoopJam.h ../multimediacycle-osg-qt/ACMultiMediaCycleOsgQt.h)
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
		ADD_EXECUTABLE(audiocycle-loopjam WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
	ELSE ()
		IF ( APPLE )
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE MultiMediaCycle.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(multimediacycle-osg-qt_SRCS ${multimediacycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)

			ADD_EXECUTABLE(audiocycle-loopjam ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ELSE ()
			ADD_EXECUTABLE(audiocycle-loopjam ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ENDIF()
	ENDIF()

	ADD_DEPENDENCIES(audiocycle-loopjam mediacycle)
	ADD_DEPENDENCIES(audiocycle-loopjam mediacycle-io-osc)
	ADD_DEPENDENCIES(audiocycle-loopjam mediacycle-osg)
	ADD_DEPENDENCIES(audiocycle-loopjam mediacycle-osg-qt)
	ADD_DEPENDENCIES(audiocycle-loopjam multimediacycle-osg-qt)
	ADD_DEPENDENCIES(audiocycle-loopjam audioengine)
	ADD_DEPENDENCIES(audiocycle-loopjam mc_visualisation)
	ADD_DEPENDENCIES(audiocycle-loopjam mc_audio)
	ADD_DEPENDENCIES(audiocycle-loopjam mc_segmentation)
	IF ( USE_APPLE_MULTITOUCH )
		ADD_DEPENDENCIES(audiocycle-loopjam mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	#IF ( APPLE )
	#	set_target_properties(audiocycle-loopjam PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(audiocycle-loopjam mediacycle)
	TARGET_LINK_LIBRARIES(audiocycle-loopjam mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiocycle-loopjam mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(audiocycle-loopjam ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiocycle-loopjam mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(audiocycle-loopjam audioengine)
#		IF ( WIN32 )
#			TARGET_LINK_LIBRARIES(audiocycle-loopjam ws2_32 winmm uuid)
#		ENDIF ( WIN32 )
#		IF(USE_OPENAL)
#			TARGET_LINK_LIBRARIES(audiocycle-loopjam${OPENAL_LIBRARY})
#		ENDIF()
#		IF(USE_PORTAUDIO)
#			TARGET_LINK_LIBRARIES(audiocycle-loopjam ${PORTAUDIO_LIBRARIES} pthread)
#		ENDIF()
	#TARGET_LINK_LIBRARIES(audiocycle-loopjam ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-loopjam ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-loopjam ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-loopjam ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH )
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(audiocycle-loopjam mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		IF(SUPPORT_AUDIO)
			SET(USE_AUDIO ON)
		ENDIF()
		IF(SUPPORT_IMAGE)
			SET(USE_IMAGE OFF)
		ENDIF()
		IF(SUPPORT_VIDEO)
			SET(USE_VIDEO OFF)
		ENDIF()
		IF(SUPPORT_3DMODEL)
			SET(USE_3DMODEL OFF)
		ENDIF()
		IF(SUPPORT_TEXT)
			SET(USE_TEXT OFF)
		ENDIF()	

		SET(PROGNAME "audiocycle-loopjam")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "LoopJam is an interactive dancefloor for collaborative music creation, using a sound map organized by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
