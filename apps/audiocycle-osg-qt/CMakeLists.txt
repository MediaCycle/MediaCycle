SET(TARGET_NAME "audiocycle-osg-qt")
IF ( SUPPORT_AUDIO AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )

	INCLUDE_DIRECTORIES( ../../libs/gui-osg-qt)

	FILE(GLOB audiocycle-osg-qt_SRCS *.cpp)
	FILE(GLOB audiocycle-osg-qt_HDRS *.h)
	FILE(GLOB audiocycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(audiocycle-osg-qt_UIS_H ${audiocycle-osg-qt_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(audiocycle-osg-qt_MOC_SRCS ${audiocycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

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
  			SET(audiocycle-osg-qt_SRCS ${audiocycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(audiocycle-osg-qt ${OS_SPECIFIC} ${audiocycle-osg-qt_SRCS} ${audiocycle-osg-qt_HDRS} ${audiocycle-osg-qt_UIS_H} ${audiocycle-osg-qt_MOC_SRCS})

	#ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-io-osc) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-osg) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-osg-qt)
	#ADD_DEPENDENCIES(audiocycle-osg-qt audioengine) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(audiocycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(audiocycle-osg-qt mc_audio)
	ADD_DEPENDENCIES(audiocycle-osg-qt mc_segmentation)

	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-osg-qt)

	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt audioengine)
#		IF ( WIN32 )
#			TARGET_LINK_LIBRARIES(audiocycle-osg-qt ws2_32 winmm uuid)
#		ENDIF ( WIN32 )
#		IF(USE_OPENAL)
#			TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENAL_LIBRARY})
#		ENDIF()
#		IF(USE_PORTAUDIO)
#			TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${PORTAUDIO_LIBRARIES} pthread)
#		ENDIF()
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

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
		SET(USE_AUDIO ON)
		SET(USE_IMAGE OFF)
		SET(USE_VIDEO OFF)
		SET(USE_3DMODEL OFF)
		SET(USE_TEXT OFF)
		SET(USE_ARCHIPEL OFF)

		SET(PROGNAME "audiocycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "AudioCycle allows to organize audio files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()
	
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
