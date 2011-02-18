SET(TARGET_NAME "multimediacycle-osg-qt")
IF(USE_QT4 AND USE_OSG AND USE_OSC AND OPENAL_FOUND)

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt ../../tinyxml)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ELSE()
		IF (WIN32)
			SET(APP_TYPE WIN32)
		ENDIF()
	ENDIF()

	SET(multimediacycle-osg-qt_SRCS main.cpp ACMultiMediaCycleOsgQt.cpp)
	SET(multimediacycle-osg-qt_MOC_HDRS ACMultiMediaCycleOsgQt.h)
	SET(multimediacycle-osg-qt_UIS ACMultiMediaCycleOsgQt.ui)

	SET(settings_SRCS settings.cpp pluginsTreeModel.cpp pluginsTreeItem.cpp)
	SET(settings_MOC_HDRS settings.h pluginsTreeModel.h pluginsTreeItem.h)
	SET(settings_UIS settings.ui)

	# Generates ui_ACMultiMediaCycleOsgQt.h  and ui_settings.h
	QT4_WRAP_UI(multimediacycle-osg-qt_UIS_H ${multimediacycle-osg-qt_UIS})
	QT4_WRAP_UI(settings_UIS_H ${settings_UIS})

	# Generates moc_ACMultiMediaCycleOsgQt.cxx and moc_settings.cxx
	QT4_WRAP_CPP(multimediacycle-osg-qt_MOC_SRCS ${multimediacycle-osg-qt_MOC_HDRS})
	QT4_WRAP_CPP(settings_MOC_SRCS ${settings_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/gui/osg-qt)

	IF ( WIN32 )
		ADD_EXECUTABLE(multimediacycle-osg-qt WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
	ELSE ()
		IF ( APPLE )
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE AudioGarden.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../audiogarden-osg-qt/AudioGarden.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(multimediacycle-osg-qt_SRCS ${multimediacycle-osg-qt_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../audiogarden-osg-qt/AudioGarden.icns)

			ADD_EXECUTABLE(multimediacycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
		ELSE ()
			ADD_EXECUTABLE(multimediacycle-osg-qt ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${multimediacycle-osg-qt_MOC_HDRS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
		ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(multimediacycle-osg-qt oscpack)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg-qt)
	
	# Plugins
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt audioengine)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_audio)
		IF(APPLE)
			#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_audio) # hack to get the plugin copied into the app bundle on OSX
		ENDIF()
	ENDIF()
	IF(SUPPORT_IMAGE OR SUPPORT_VIDEO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_image)
		IF(APPLE)
			#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_video) # hack to get the plugin copied into the app bundle on OSX
		ENDIF()
	ENDIF()
	IF(SUPPORT_VIDEO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_video)
		IF(APPLE)
			#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_video) # hack to get the plugin copied into the app bundle on OSX
		ENDIF()
	ENDIF()
	IF(SUPPORT_3DMODEL)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_3Dmodel)
		IF(APPLE)
			#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_3Dmodel) # hack to get the plugin copied into the app bundle on OSX
		ENDIF()
	ENDIF()
	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_visualisation)
	IF(APPLE)
		#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_segmentation) # hack to get the plugin copied into the app bundle on OSX
		#TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mc_visualisation) # hack to get the plugin copied into the app bundle on OSX
	ENDIF()

	#IF ( APPLE )
	#	set_target_properties(multimediacycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${QT_LIBRARIES})
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(multimediacycle-osg-qt audioengine)
	ENDIF()
	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )

	IF(APPLE AND NOT USE_DEBUG)
		IF(SUPPORT_AUDIO)
			SET(USE_AUDIO ON)
		ENDIF()
		IF(SUPPORT_IMAGE)
			SET(USE_IMAGE ON)
		ENDIF()
		IF(SUPPORT_VIDEO)
			SET(USE_VIDEO ON)
		ENDIF()
		IF(SUPPORT_3DMODEL)
			SET(USE_3DMODEL ON)
		ENDIF()		

		SET(PROGNAME "multimediacycle-osg-qt")
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/ConfigureAppleBundle.cmake)
	ENDIF(APPLE AND NOT USE_DEBUG)

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
