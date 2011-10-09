SET(TARGET_NAME "imagecycle-osg-qt")
IF ( SUPPORT_IMAGE AND USE_QT4 AND USE_OSG AND USE_OSC )

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
		ADD_EXECUTABLE(imagecycle-osg-qt WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
	ELSE ()
		IF ( APPLE )
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE MultiMediaCycle.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(multimediacycle-osg-qt_SRCS ${multimediacycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)

			ADD_EXECUTABLE(imagecycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ELSE ()
			ADD_EXECUTABLE(imagecycle-osg-qt ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ENDIF()
	ENDIF()

	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(imagecycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(imagecycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(imagecycle-osg-qt mc_image)
	#ADD_DEPENDENCIES(imagecycle-osg-qt mc_segmentation)

	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(imagecycle-osg-qt mediacycle-io-osc)
	#TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(imagecycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	ENDIF ( APPLE )
	
	# Packaging
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		SET(USE_AUDIO OFF)
		SET(USE_IMAGE ON)
		SET(USE_VIDEO OFF)
		SET(USE_3DMODEL OFF)
		SET(USE_TEXT OFF)
		SET(USE_ARCHIPEL OFF)

		SET(PROGNAME "imagecycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "MultiMediaCycle allows to organize media files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
