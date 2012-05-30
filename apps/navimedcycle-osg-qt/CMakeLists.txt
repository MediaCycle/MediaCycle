SET(TARGET_NAME "navimedcycle-osg-qt")
IF ( SUPPORT_NAVIMED AND SUPPORT_MULTIMEDIA AND SUPPORT_IMAGE AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG )

	INCLUDE_DIRECTORIES( ../../libs/gui-osg-qt)

	FILE(GLOB navimedcycle-osg-qt_SRCS *.cpp)
	FILE(GLOB navimedcycle-osg-qt_HDRS *.h)
	FILE(GLOB navimedcycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(navimedcycle-osg-qt_UIS_H ${navimedcycle-osg-qt_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(navimedcycle-osg-qt_MOC_SRCS ${navimedcycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

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
  			SET(navimedcycle-osg-qt_SRCS ${navimedcycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(navimedcycle-osg-qt ${OS_SPECIFIC} ${navimedcycle-osg-qt_SRCS} ${navimedcycle-osg-qt_HDRS} ${navimedcycle-osg-qt_UIS_H} ${navimedcycle-osg-qt_MOC_SRCS})

	#ADD_DEPENDENCIES(navimedcycle-osg-qt mediacycle) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(navimedcycle-osg-qt mediacycle-osg) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(navimedcycle-osg-qt mediacycle-io-osc) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(navimedcycle-osg-qt mediacycle-osg-qt)
	IF(SUPPORT_AUDIO)
		ADD_DEPENDENCIES(navimedcycle-osg-qt audioengine)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_audio)
	ENDIF()
	IF(SUPPORT_IMAGE OR SUPPORT_VIDEO)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_image)
	ENDIF()
	IF(SUPPORT_VIDEO)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_video)
	ENDIF()
	IF(SUPPORT_3DMODEL)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_3Dmodel)
	ENDIF()
	IF(SUPPORT_TEXT)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_text )
	ENDIF()
	ADD_DEPENDENCIES(navimedcycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(navimedcycle-osg-qt mc_visualisation)
	if (SUPPORT_NAVIMED)
		ADD_DEPENDENCIES(navimedcycle-osg-qt mc_navimed)
	ENDIF()	
	
	TARGET_LINK_LIBRARIES(navimedcycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(navimedcycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(navimedcycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(navimedcycle-osg-qt audioengine)
		IF ( WIN32 )
			TARGET_LINK_LIBRARIES(navimedcycle-osg-qt ws2_32 winmm uuid) # 2 last from OpenAL pkg config
		ENDIF ( WIN32 )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	
	# Packaging
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		SET(USE_AUDIO ON)
		SET(USE_IMAGE ON)
		SET(USE_VIDEO ON)
		SET(USE_3DMODEL ON)
		SET(USE_TEXT ON)
		SET(USE_NAVIMED ON)

		SET(PROGNAME "navimedcycle-osg-qt")
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "NavimedCycle is an attempt to complement the Navimed database from Polymedis (Belgium) with the MediaCycle organization of media files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()