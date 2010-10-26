IF ( APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE )
	IF (OPENAL_FOUND AND ALUT_FOUND)
		set(NAME audiocycle-osg-ipad)

		FILE(GLOB SRC *.m *.mm)
		FILE(GLOB HDR *.h)

		INCLUDE_DIRECTORIES(. ../../gui/osg)

		SET(APP_TYPE MACOSX_BUNDLE)
		ADD_EXECUTABLE(audiocycle-osg-ipad ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST} ${SRC} ${HDR})

		ADD_DEPENDENCIES(audiocycle-osg-ipad mediacycle)
		ADD_DEPENDENCIES(audiocycle-osg-ipad mediacycle-osg)
		ADD_DEPENDENCIES(audiocycle-osg-ipad audioengine)
		set_target_properties(audiocycle-osg-ipad PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/apps/audiocycle-osg-ipad/Info.plist)

		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioToolbox -framework CoreGraphics -framework CoreFoundation -framework Foundation -framework QuartzCore -framework UIKit -framework OpenGLES -framework OpenAL")
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad mediacycle)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad OpenThreads osg osgDB osgGA osgUtil osgViewer)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad mediacycle-osg)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad audioengine)
		TARGET_LINK_LIBRARIES(audiocycle-osg-ipad ${ALUT_LIBRARY})

		# Dirty osgPlugins hack
		IF( APPLE_IOS_DEV OR APPLE_IOS_SIM )
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${CMAKE_INSTALL_PREFIX}/lib/osgPlugins-2.9.10/ -losgdb_osg -losgdb_imageio")
		ELSEIF( APPLE_IOS_XCODE )
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${iPhoneSimulator_DEPS_ROOT}/lib/osgPlugins-2.9.10/ -losgdb_osg -losgdb_imageio")
		ENDIF()

		add_custom_command(
    		TARGET audiocycle-osg-ipad
   			POST_BUILD
    			#COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn -resolve-src-symlinks ${RES_DIR} ${APP_NAME}
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}-iphonesimulator/audiocycle-osg-ipad.app/ 
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../data/audio/zero-g-pro-pack_a/Brass\ Elements/Brass\ 076\ BPM/* ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}-iphonesimulator/audiocycle-osg-ipad.app/
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/Resources/* ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}-iphonesimulator/audiocycle-osg-ipad.app/ 
		)
	ENDIF (OPENAL_FOUND AND ALUT_FOUND)
ENDIF ( APPLE_IOS_DEV OR APPLE_IOS_SIM OR APPLE_IOS_XCODE )
