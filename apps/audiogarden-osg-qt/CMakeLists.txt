SET(TARGET_NAME "audiogarden-osg-qt")
IF (SUPPORT_AUDIO AND USE_AUDIOGARDEN AND USE_QT4 AND USE_OSG AND USE_OSC AND ( OPENAL_FOUND OR PORTAUDIO_FOUND ) )
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(audiogarden-osg-qt_SRCS main.cpp ACAudioGardenOsgQt.cpp)
	SET(audiogarden-osg-qt_MOC_HDRS ACAudioGardenOsgQt.h)
	SET(audiogarden-osg-qt_UIS ACAudioGardenOsgQt.ui)

	# Generates ui_ACAudioGardenOsgQt.h
	QT4_WRAP_UI(audiogarden-osg-qt_UIS_H ${audiogarden-osg-qt_UIS})

	# Generates moc_ACAudioGardenOsgQt.cxx
	QT4_WRAP_CPP(audiogarden-osg-qt_MOC_SRCS ${audiogarden-osg-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(audiogarden-osg-qt WIN32 ${audiogarden-osg-qt_SRCS} ${audiogarden-osg-qt_UIS_H} ${audiogarden-osg-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(audiogarden-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${audiogarden-osg-qt_SRCS} ${audiogarden-osg-qt_UIS_H} ${audiogarden-osg-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(audiogarden-osg-qt ${audiogarden-osg-qt_SRCS} ${audiogarden-osg-qt_UIS_H} ${audiogarden-osg-qt_MOC_SRCS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(audiogarden-osg-qt mediacycle)
	ADD_DEPENDENCIES(audiogarden-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(audiogarden-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(audiogarden-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(audiogarden-osg-qt audioengine)
	ADD_DEPENDENCIES(audiogarden-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(audiogarden-osg-qt mc_audio)
	ADD_DEPENDENCIES(audiogarden-osg-qt mc_segmentation)
	IF ( USE_APPLE_MULTITOUCH )
		ADD_DEPENDENCIES(audiogarden-osg-qt mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	ADD_DEPENDENCIES(audiogarden-osg-qt armadillo-utils audiofeatures audiogarden-synthesis)

	#IF ( APPLE )
	#	set_target_properties(audiogarden-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle-osg-qt)
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(audiogarden-osg-qt audioengine)
	TARGET_LINK_LIBRARIES(audiogarden-osg-qt audiofeatures audiogarden-synthesis ${SNDFILE_LIBRARY} ${SAMPLERATE_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${OPENAL_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${OPENGL_LIBRARIES}
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiogarden-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH )
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(audiogarden-osg-qt mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
