SET(TARGET_NAME "multimediacycle-osg-qt")
IF(USE_QT4 AND USE_OSG AND USE_OSC )

	INCLUDE_DIRECTORIES( ../../libs/gui-osg-qt)

	FILE(GLOB multimediacycle-osg-qt_SRCS *.cpp)
	FILE(GLOB multimediacycle-osg-qt_HDRS *.h)
	FILE(GLOB multimediacycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(multimediacycle-osg-qt_UIS_H ${multimediacycle-osg-qt_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(multimediacycle-osg-qt_MOC_SRCS ${multimediacycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

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
  			SET(multimediacycle-osg-qt_SRCS ${multimediacycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(multimediacycle-osg-qt ${OS_SPECIFIC} ${multimediacycle-osg-qt_SRCS} ${multimediacycle-osg-qt_HDRS} ${multimediacycle-osg-qt_UIS_H} ${multimediacycle-osg-qt_MOC_SRCS})

	#ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-io-osc) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-osg-qt)
	IF ( USE_APPLE_MULTITOUCH AND NOT USE_DEBUG )
		ADD_DEPENDENCIES(multimediacycle-osg-qt mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH AND NOT USE_DEBUG )
	
	# Plugins
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt audioengine)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_audio)
	ENDIF()
	IF(SUPPORT_IMAGE OR SUPPORT_VIDEO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_image)
	ENDIF()
	IF(SUPPORT_VIDEO)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_video)
	ENDIF()
	IF(SUPPORT_3DMODEL)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_3Dmodel)
	ENDIF()
	IF(SUPPORT_TEXT)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_text mc_text_sparse mc_SparseVisualisation)
	ENDIF()
	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(multimediacycle-osg-qt mc_visualisation)
	if (SUPPORT_ARCHIPEL)
		ADD_DEPENDENCIES(multimediacycle-osg-qt mc_archipel)
	ENDIF()

	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${QT_LIBRARIES})
	IF(SUPPORT_AUDIO)
		TARGET_LINK_LIBRARIES(multimediacycle-osg-qt audioengine)
		IF ( WIN32 )
			TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ws2_32 winmm uuid) # 2 last from OpenAL pkg config
		ENDIF ( WIN32 )
		IF(USE_OPENAL)
			TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${OPENAL_LIBRARY})
		ENDIF()
		IF(USE_PORTAUDIO)
			TARGET_LINK_LIBRARIES(multimediacycle-osg-qt ${PORTAUDIO_LIBRARIES} pthread)
		ENDIF()
	ENDIF()
	
	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH AND NOT USE_DEBUG)
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(multimediacycle-osg-qt mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH AND NOT USE_DEBUG )
	ENDIF ( APPLE )

	# Packaging
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
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
		IF(SUPPORT_TEXT)
			SET(USE_TEXT ON)
		ENDIF()	
		IF(SUPPORT_ARCHIPEL)
			SET(USE_ARCHIPEL ON)
		ENDIF()	

		SET(PROGNAME "multimediacycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "MultiMediaCycle allows to visually organize media files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
