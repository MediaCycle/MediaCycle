IF ( USE_QT4 AND USE_OSC AND USE_OPENAL )

	MESSAGE("apps/audiocycle-osg-qt")

	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../gui/osg ../../gui/osg-qt)
	INCLUDE_DIRECTORIES(../../io/osc ../../io/osc/oscpack)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(audiocycle-osg-qt_SRCS main.cpp ACAudioCycleOsgQt.cpp)
	SET(audiocycle-osg-qt_MOC_HDRS ACAudioCycleOsgQt.h)
	SET(audiocycle-osg-qt_UIS ACAudioCycleOsgQt.ui)

	# Generates ui_ACAudioCycleOsgQt.h
	QT4_WRAP_UI(audiocycle-osg-qt_UIS_H ${audiocycle-osg-qt_UIS})

	# Generates moc_ACAudioCycleOsgQt.cxx
	QT4_WRAP_CPP(audiocycle-osg-qt_MOC_SRCS ${audiocycle-osg-qt_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(audiocycle-osg-qt WIN32 ${audiocycle-osg-qt_SRCS} ${audiocycle-osg-qt_UIS_H} ${audiocycle-osg-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(audiocycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${audiocycle-osg-qt_SRCS} ${audiocycle-osg-qt_UIS_H} ${audiocycle-osg-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(audiocycle-osg-qt ${audiocycle-osg-qt_SRCS} ${audiocycle-osg-qt_UIS_H} ${audiocycle-osg-qt_MOC_SRCS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-io-osc)
	ADD_DEPENDENCIES(audiocycle-osg-qt oscpack)
	ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(audiocycle-osg-qt mediacycle-osg-qt)
	ADD_DEPENDENCIES(audiocycle-osg-qt audioengine)

	#IF ( APPLE )
	#	set_target_properties(audiocycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-osg-qt)
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt osg osgDB osgGA osgViewer osgText osgUtil osgText)
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt OpenThreads)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt mediacycle-io-osc)
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt oscpack)
	TARGET_LINK_LIBRARIES(audiocycle-osg-qt audioengine)
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENAL_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${QT_LIBRARIES})
	#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	ENDIF ( APPLE )

	IF ( WIN32 )
		#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENGL_LIBRARIES} ${FREEGLUT_LIBRARIES})
	ELSE ( WIN32 )
		#TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
	ENDIF ( WIN32 ) 

	IF ( APPLE AND NOT SNOW_LEOPARD )
		MESSAGE("ALUT not yet required.")
	ELSE ()
		IF ( ALUT_FOUND)
			TARGET_LINK_LIBRARIES(audiocycle-osg-qt ${ALUT_LIBRARY})
		ELSE ( ALUT_FOUND)
			MESSAGE(FATAL_ERROR "ALUT requested but not found.")
		ENDIF ( ALUT_FOUND)
	ENDIF ()
	

ELSE ( )
	MESSAGE("Qt4 and/or OpenAL and/or OSC disabled - not compiling AudioCycle OSG Qt GUI.")
ENDIF ( )
