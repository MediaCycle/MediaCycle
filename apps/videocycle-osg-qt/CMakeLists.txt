IF ( USE_QT4 AND USE_OSG AND USE_OSC AND OPENAL_FOUND )

	MESSAGE("apps/videocycle-osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt)
	INCLUDE_DIRECTORIES(../../io/osc ../../io/osc/oscpack)

	IF ( USE_APPLE_MULTITOUCH )
		INCLUDE_DIRECTORIES( ../../io/applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(videocycle-osg-qt_SRCS main.cpp ACVideoCycleOsgQt.cpp)
	SET(videocycle-osg-qt_MOC_HDRS ACVideoCycleOsgQt.h)
	SET(videocycle-osg-qt_UIS ACVideoCycleOsgQt.ui)

	# Generates ui_ACVideoCycleOsgQt.h
	QT4_WRAP_UI(videocycle-osg-qt_UIS_H ${videocycle-osg-qt_UIS})

	# Generates moc_ACVideoCycleOsgQt.cxx
	QT4_WRAP_CPP(videocycle-osg-qt_MOC_SRCS ${videocycle-osg-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(videocycle-osg-qt WIN32 ${videocycle-osg-qt_SRCS} ${videocycle-osg-qt_UIS_H} ${videocycle-osg-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(videocycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${videocycle-osg-qt_SRCS} ${videocycle-osg-qt_UIS_H} ${videocycle-osg-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(videocycle-osg-qt ${videocycle-osg-qt_SRCS} ${videocycle-osg-qt_UIS_H} ${videocycle-osg-qt_MOC_SRCS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(videocycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(videocycle-osg-qt oscpack)
	ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(videocycle-osg-qt audioengine)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_audio)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(videocycle-osg-qt mc_video)
	IF ( USE_APPLE_MULTITOUCH )
		ADD_DEPENDENCIES(videocycle-osg-qt mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )

	#IF ( APPLE )
	#	set_target_properties(videocycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-osg-qt)
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt OpenThreads)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-io-osc)
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt oscpack)
	TARGET_LINK_LIBRARIES(videocycle-osg-qt audioengine)
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${OPENAL_LIBRARY})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${OPENGL_LIBRARIES})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(videocycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
		IF ( USE_APPLE_MULTITOUCH )
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
			TARGET_LINK_LIBRARIES(videocycle-osg-qt mediacycle-io-applemultitouch)
		ENDIF ( USE_APPLE_MULTITOUCH )
	ENDIF ( APPLE )

	#IF ( APPLE_LEOPARD )
	#	MESSAGE("ALUT not yet required.")
	#ELSE ()
	#	IF ( ALUT_FOUND)
	#		TARGET_LINK_LIBRARIES(videocycle-osg-qt ${ALUT_LIBRARY})
	#	ELSE ( ALUT_FOUND)
	#		MESSAGE(FATAL_ERROR "ALUT requested but not found.")
	#	ENDIF ( ALUT_FOUND)
	#ENDIF ()
ELSE ( )
	MESSAGE("Qt4 and/or OpenAL and/or OSG and/or OSC disabled - not compiling AudioCycle OSG Qt GUI.")
ENDIF ( )
