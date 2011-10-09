SET(TARGET_NAME "archipelcycle-osg-qt")
IF ( SUPPORT_ARCHIPEL AND SUPPORT_MULTIMEDIA AND SUPPORT_AUDIO AND SUPPORT_IMAGE AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG )

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
		ADD_EXECUTABLE(archipelcycle-osg-qt WIN32 ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
	ELSE ()
		IF ( APPLE )
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE MultiMediaCycle.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(multimediacycle-osg-qt_SRCS ${multimediacycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)

			ADD_EXECUTABLE(archipelcycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ELSE ()
			ADD_EXECUTABLE(archipelcycle-osg-qt ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS} ${settings_HS})
		ENDIF()
	ENDIF()

	ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_text mc_text_sparse mc_SparseVisualisation)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_audio mc_image mc_archipel archipel)	

	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle-osg-qt)

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	
	# Packaging
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		SET(USE_AUDIO ON)
		SET(USE_IMAGE ON)
		SET(USE_VIDEO OFF)
		SET(USE_3DMODEL OFF)
		SET(USE_TEXT ON)
		SET(USE_ARCHIPEL ON)

		SET(PROGNAME "archipelcycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "An attempt to complement the Archipel database from La Mediatheque (Belgium) with the MediaCycle organization of media files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()