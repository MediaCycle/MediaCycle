SET(TARGET_NAME "videocycle-osg-cocoa")
IF(SUPPORT_VIDEO AND APPLE AND USE_COCOA AND USE_OSC AND OPENAL_FOUND)
	FILE(GLOB SRC *.m *.mm *.nib)
	FILE(GLOB HDR *.h)

	SET(APP_TYPE MACOSX_BUNDLE)
	ADD_EXECUTABLE(videocycle-osg-cocoa ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})
	ADD_DEPENDENCIES(videocycle-osg-cocoa mediacycle-io-osc)
	ADD_DEPENDENCIES(videocycle-osg-cocoa mediacycle-osg-cocoa)
	ADD_DEPENDENCIES(videocycle-osg-cocoa mc_visualisation)
	ADD_DEPENDENCIES(videocycle-osg-cocoa mc_visualisation-advanced)
	ADD_DEPENDENCIES(videocycle-osg-cocoa mc_video)
	ADD_DEPENDENCIES(videocycle-osg-cocoa mc_segmentation)
	set_target_properties(videocycle-osg-cocoa PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/apps/videocycle-osg-cocoa/Info.plist)
	set_source_files_properties(MainMenu.nib PROPERTIES MACOSX_PACKAGE_LOCATION Resources) # doesn't seem to work if compiling outside XCode
	IF(NOT XCODE)
		add_custom_command(
    			TARGET videocycle-osg-cocoa
   			POST_BUILD
			COMMAND cp -R ${CMAKE_CURRENT_SOURCE_DIR}/MainMenu.nib ${CMAKE_CURRENT_BINARY_DIR}/videocycle-osg-cocoa.app/Contents/Resources/
		)
	ENDIF()
	TARGET_LINK_LIBRARIES(videocycle-osg-cocoa mediacycle-osg-cocoa)
	TARGET_LINK_LIBRARIES(videocycle-osg-cocoa ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(videocycle-osg-cocoa mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(videocycle-osg-cocoa ${OPENGL_LIBRARIES})
	TARGET_LINK_LIBRARIES(videocycle-osg-cocoa ${OPENAL_LIBRARY})

	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
