SET(TARGET_NAME "imagecycle-osg-cocoa")
IF(SUPPORT_IMAGE AND APPLE AND USE_COCOA AND USE_OSC)
	FILE(GLOB SRC *.m *.mm *.nib)
	FILE(GLOB HDR *.h)

	INCLUDE_DIRECTORIES(. ../../core ../../gui/osg ../../gui/osg-cocoa)
	INCLUDE_DIRECTORIES(../../io/osc ../../io/osc/oscpack)

	SET(APP_TYPE MACOSX_BUNDLE)
	ADD_EXECUTABLE(imagecycle-osg-cocoa ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mediacycle)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mediacycle-io-osc)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa oscpack)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mediacycle-osg)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mediacycle-osg-cocoa)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mc_visualisation)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mc_image)
	ADD_DEPENDENCIES(imagecycle-osg-cocoa mc_segmentation)
	set_target_properties(imagecycle-osg-cocoa PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/apps/imagecycle-osg-cocoa/Info.plist)
	set_source_files_properties(MainMenu.nib PROPERTIES MACOSX_PACKAGE_LOCATION Resources) # doesn't seem to work if compiling outside XCode
	IF(NOT XCODE)
		add_custom_command(
    			TARGET imagecycle-osg-cocoa
   			POST_BUILD
			COMMAND cp -R ${CMAKE_CURRENT_SOURCE_DIR}/MainMenu.nib ${CMAKE_CURRENT_BINARY_DIR}/imagecycle-osg-cocoa.app/Contents/Resources/
		)
	ENDIF()
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa mediacycle)
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa mediacycle-osg)
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa mediacycle-osg-cocoa)
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa ${OPENSCENEGRAPH_LIBRARIES})
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa mediacycle-io-osc)
	TARGET_LINK_LIBRARIES(imagecycle-osg-cocoa ${OPENGL_LIBRARIES})

	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreVideo -framework AppKit -framework Foundation")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()