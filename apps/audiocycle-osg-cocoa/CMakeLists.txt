SET(TARGET_NAME "audiocycle-osg-cocoa")
IF ( SUPPORT_AUDIO AND APPLE AND USE_COCOA AND USE_OSC AND OPENAL_FOUND)
	FILE(GLOB SRC *.m *.mm *.nib)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../core ../../gui/osg ../../gui/osg-cocoa)
	INCLUDE_DIRECTORIES(../../io/osc ../../io/osc/oscpack)

	SET(APP_TYPE MACOSX_BUNDLE)
	ADD_EXECUTABLE(audiocycle-osg-cocoa ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mediacycle)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mediacycle-io-osc)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa oscpack)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mediacycle-osg)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mediacycle-osg-cocoa)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa audioengine)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mc_visualisation)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mc_audio)
	ADD_DEPENDENCIES(audiocycle-osg-cocoa mc_segmentation)
	set_target_properties(audiocycle-osg-cocoa PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/apps/audiocycle-osg-cocoa/Info.plist)
	set_source_files_properties(MainMenu.nib PROPERTIES MACOSX_PACKAGE_LOCATION Resources) # doesn't seem to work if compiling outside XCode
	IF(NOT XCODE)
		add_custom_command(
    			TARGET audiocycle-osg-cocoa
   			POST_BUILD
			COMMAND cp -R ${CMAKE_CURRENT_SOURCE_DIR}/MainMenu.nib ${CMAKE_CURRENT_BINARY_DIR}/audiocycle-osg-cocoa.app/Contents/Resources/
		)
	ENDIF()
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa mediacycle)
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa mediacycle-osg)
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa mediacycle-osg-cocoa)
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa audioengine)
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa ${OPENGL_LIBRARIES})
	TARGET_LINK_LIBRARIES(audiocycle-osg-cocoa ${OPENAL_LIBRARY})

	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	MESSAGE("[X] audiocycle-osg-cocoa")
ELSE()
	MESSAGE("[ ] audiocycle-osg-cocoa")
ENDIF()
