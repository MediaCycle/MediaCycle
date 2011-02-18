SET(TARGET_NAME "audiocycle-osg-ipad")
IF(APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE)
	IF (SUPPORT_AUDIO AND OPENAL_FOUND)
		set(NAME audiocycle-osg-ipad)

		FILE(GLOB SRC *.m *.mm *.xib)
		FILE(GLOB HDR *.h)

		INCLUDE_DIRECTORIES(. ../../core ../../gui/osg)

		SET(APP_TYPE MACOSX_BUNDLE)
		ADD_EXECUTABLE(audiocycle-osg-ipad ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})

		ADD_DEPENDENCIES(audiocycle-osg-ipad mediacycle)
		ADD_DEPENDENCIES(audiocycle-osg-ipad mediacycle-osg)
		ADD_DEPENDENCIES(audiocycle-osg-ipad audioengine)
		set_source_files_properties(MainWindowiPad.xib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set_source_files_properties(ACOsgiPadControlleriPad.xib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		set_target_properties(audiocycle-osg-ipad PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/apps/audiocycle-osg-ipad/Info.plist)

		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioToolbox -framework CoreGraphics -framework CoreFoundation -framework Foundation -framework QuartzCore -framework UIKit -framework OpenGLES -framework OpenAL")
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad mediacycle)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad ${OPENSCENEGRAPH_LIBRARIES})
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad mediacycle-osg)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad audioengine)

		# Dirty osgPlugins hack
		IF( APPLE_IOS_DEV OR APPLE_IOS_SIM )
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_INSTALL_PREFIX}/lib/osgPlugins-2.9.11/ -losgdb_osg -losgdb_imageio -lz -lbz2")
		ELSEIF( APPLE_IOS_XCODE )
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${iPhoneSimulator_DEPS_ROOT}/lib/osgPlugins-2.9.11/ -losgdb_osg -losgdb_imageio -lz -lbz2")
		ENDIF()

		add_custom_command(
    		TARGET audiocycle-osg-ipad
   			POST_BUILD
    			#COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn -resolve-src-symlinks ${RES_DIR} ${APP_NAME}
			#COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist "${CMAKE_CURRENT_BINARY_DIR}/Release/audiocycle-osg-ipad.app/"
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../data/audio/zero-g-pro-pack_a/Brass\ Elements/Brass\ 076\ BPM/* ${CMAKE_CURRENT_BINARY_DIR}/Release/audiocycle-osg-ipad.app/
			#COMMAND cp /AudioCycleProPackTest/zero-g-pro-pack_*/*/*/* ${CMAKE_CURRENT_BINARY_DIR}/Release/audiocycle-osg-ipad.app/
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Resources/* ${CMAKE_CURRENT_BINARY_DIR}/Release/audiocycle-osg-ipad.app/ 
		)
	ENDIF()
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
