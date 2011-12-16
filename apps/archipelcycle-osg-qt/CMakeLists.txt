SET(TARGET_NAME "archipelcycle-osg-qt")
IF ( SUPPORT_ARCHIPEL AND SUPPORT_MULTIMEDIA AND SUPPORT_AUDIO AND SUPPORT_IMAGE AND SUPPORT_TEXT AND USE_QT4 AND USE_OSG )

	INCLUDE_DIRECTORIES( ../../libs/gui-osg-qt)

	FILE(GLOB archipelcycle-osg-qt_SRCS *.cpp)
	FILE(GLOB archipelcycle-osg-qt_HDRS *.h)
	FILE(GLOB archipelcycle-osg-qt_UIS *.ui)

	# Generates ui_*.h files
	QT4_WRAP_UI(archipelcycle-osg-qt_UIS_H ${archipelcycle-osg-qt_UIS})

	# Generates moc_*.cxx files
	QT4_WRAP_CPP(archipelcycle-osg-qt_MOC_SRCS ${archipelcycle-osg-qt_HDRS} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

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
  			SET(archipelcycle-osg-qt_SRCS ${archipelcycle-osg-qt_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/MultiMediaCycle.icns)
			SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
		ENDIF()
	ENDIF()
	ADD_EXECUTABLE(archipelcycle-osg-qt ${OS_SPECIFIC} ${archipelcycle-osg-qt_SRCS} ${archipelcycle-osg-qt_HDRS} ${archipelcycle-osg-qt_UIS_H} ${archipelcycle-osg-qt_MOC_SRCS})

	#ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle-osg) # mediacycle-osg-qt depends on it
	#ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle-io-osc) # mediacycle-osg-qt depends on it
	ADD_DEPENDENCIES(archipelcycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_text mc_text_sparse mc_SparseVisualisation)
	ADD_DEPENDENCIES(archipelcycle-osg-qt audioengine)
	ADD_DEPENDENCIES(archipelcycle-osg-qt mc_audio mc_image mc_archipel archipel)	

	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(archipelcycle-osg-qt audioengine)
		IF ( WIN32 )
			TARGET_LINK_LIBRARIES(archipelcycle-osg-qt ws2_32 winmm uuid) # 2 last from OpenAL pkg config
		ENDIF ( WIN32 )
		IF(USE_OPENAL)
			TARGET_LINK_LIBRARIES(archipelcycle-osg-qt ${OPENAL_LIBRARY})
		ENDIF()
		IF(USE_PORTAUDIO)
			TARGET_LINK_LIBRARIES(archipelcycle-osg-qt ${PORTAUDIO_LIBRARIES} pthread)
		ENDIF()

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
        SET(MC_PACKAGE_DESCRIPTION_SUMMARY "ArchipelCycle is an attempt to complement the Archipel database from La Mediatheque (Belgium) with the MediaCycle organization of media files by similarity.")
		SET(WITH_QT4 ON)
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()