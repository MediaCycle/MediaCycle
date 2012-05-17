SET(TARGET_NAME "videocycle-osg-qt")
IF ( SUPPORT_VIDEO AND USE_QT4 AND USE_OSG AND USE_OSC )

	INCLUDE_DIRECTORIES( ../../libs/gui-osg-qt)

	FILE(GLOB videocycle-osg-qt_SRCS *.cpp)
	FILE(GLOB videocycle-osg-qt_HDRS *.h)
	FILE(GLOB videocycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(videocycle-osg-qt_UIS_H ${videocycle-osg-qt_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(videocycle-osg-qt_MOC_SRCS ${videocycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)

	IF ( WIN32 )
		SET(OS_SPECIFIC WIN32)
	ELSE ()
		IF ( APPLE )
			SET(APP_TYPE MACOSX_BUNDLE)
			SET(MACOSX_BUNDLE_BUNDLE_NAME ${TARGET_NAME})
			# set how it shows up in the Info.plist file
  			SET(MACOSX_BUNDLE_ICON_FILE MultiMediaCycle.icns) 
  			# set where in the bundle to put the icns file
  			SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  			# include the icns file in the target
  			SET(videocycle-osg-qt_SRCS ${videocycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(videocycle-osg-qt ${OS_SPECIFIC} ${videocycle-osg-qt_SRCS} ${videocycle-osg-qt_HDRS} ${videocycle-osg-qt_UIS_H} ${videocycle-osg-qt_MOC_SRCS})

	#ADD_DEPENDENCIES(videocycle-osg-qt mediacycle) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-osg) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-io-osc) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-osg-qt)
	#IF(SUPPORT_AUDIO)
		#ADD_DEPENDENCIES(videocycle-osg-qt audioengine) # mediacycle-osg-qt depends on it
	#ENDIF()
	ADD_DEPENDENCIES(videocycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_video)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_segmentation)

	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-io-osc)
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(videocycle-osg-qt audioengine)
	ENDIF()
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${OPENAL_LIBRARY})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	ENDIF ( APPLE )

	# Copy Qt imageformat plugin to (Apple) Debug apps
	IF(APPLE AND USE_DEBUG)
		ADD_CUSTOM_COMMAND( TARGET ${TARGET_NAME} POST_BUILD
			COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.app/Contents/plugins
      			COMMAND cp -R ${QT_PLUGINS_DIR}/imageformats ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.app/Contents/plugins
		)
	ENDIF()
	
	# Packaging
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		SET(USE_AUDIO OFF)
		SET(USE_IMAGE OFF)
		SET(USE_VIDEO ON)
		SET(USE_3DMODEL OFF)
		SET(USE_TEXT OFF)
		SET(USE_ARCHIPEL OFF)

		SET(PROGNAME "videocycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "VideoCycle allows to visually organize video files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
