SET(TARGET_NAME "textcycle-osg-qt")
IF(SUPPORT_TEXT AND USE_QT4)
	FILE(GLOB SRC *.cpp)
	FILE(GLOB HDR *.h)

	IF ( APPLE )
		SET(APP_TYPE MACOSX_BUNDLE)
	ENDIF ( APPLE )

	SET(textcycle-osg-qt_SRCS main.cpp ACTextCycleOsgQt.cpp)
	SET(textcycle-osg-qt_MOC_HDRS ACTextCycleOsgQt.h)
	SET(textcycle-osg-qt_UIS ACTextCycleOsgQt.ui)

	SET(settings_SRCS settings.cpp)
	SET(settings_MOC_HDRS settings.h)
	SET(settings_UIS settings.ui)

	# Generates ui_ACTextCycleOsgQt.h  and ui_settings.h
	QT4_WRAP_UI(textcycle-osg-qt_UIS_H ${textcycle-osg-qt_UIS})
	QT4_WRAP_UI(settings_UIS_H ${settings_UIS})

	# Generates moc_ACTextCycleOsgQt.cxx and moc_settings.cxx
	QT4_WRAP_CPP(textcycle-osg-qt_MOC_SRCS ${textcycle-osg-qt_MOC_HDRS})
	QT4_WRAP_CPP(settings_MOC_SRCS ${settings_MOC_HDRS})

	# Don't forget to include output directory, otherwise
	# the UI file won't be wrapped!
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	IF ( WIN32 )
		ADD_EXECUTABLE(textcycle-osg-qt WIN32 ${textcycle-osg-qt_SRCS} ${textcycle-osg-qt_UIS_H} ${textcycle-osg-qt_MOC_SRCS})
	ELSE ()
		#IF ( APPLE )
		#	ADD_EXECUTABLE(textcycle-osg-qt ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${textcycle-osg-qt_SRCS} ${textcycle-osg-qt_UIS_H} ${textcycle-osg-qt_MOC_SRCS})
		#ELSE ()
			ADD_EXECUTABLE(textcycle-osg-qt ${textcycle-osg-qt_SRCS} ${textcycle-osg-qt_UIS_H} ${textcycle-osg-qt_MOC_SRCS} ${textcycle-osg-qt_MOC_HDRS} ${settings_SRCS} ${settings_UIS_H} ${settings_MOC_SRCS} ${settings_MOC_HDRS})
		#ENDIF ()
	ENDIF ( )

	ADD_DEPENDENCIES(textcycle-osg-qt mc_text)
	ADD_DEPENDENCIES(textcycle-osg-qt mc_segmentation)
	ADD_DEPENDENCIES(textcycle-osg-qt mc_visualisation)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-osg)
	ADD_DEPENDENCIES(textcycle-osg-qt mediacycle-osg-qt)

	#IF ( APPLE )
	#	set_target_properties(textcycle-osg-qt PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/gui/osg-qt/Info.plist)
	#	set_source_files_properties(PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	#ENDIF ( APPLE )

	#TARGET_LINK_LIBRARIES(textcycle-osg-qt mc_text)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-osg)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt mediacycle-osg-qt)
	TARGET_LINK_LIBRARIES(textcycle-osg-qt ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(textcycle-osg-qt ${Boost_SYSTEM_LIBRARY})
	TARGET_LINK_LIBRARIES(textcycle-osg-qt ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(textcycle-osg-qt ${QT4_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} )

	IF ( APPLE )
		set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	ENDIF ( APPLE )
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
