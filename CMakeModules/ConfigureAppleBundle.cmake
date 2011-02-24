#From: http://www.cmake.org/Wiki/BundleUtilitiesExample
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGNAME}")
IF(APPLE)
  SET(plugin_dest_dir ${PROGNAME}.app/Contents/MacOS)
  SET(qtconf_dest_dir ${PROGNAME}.app/Contents/Resources)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGNAME}.exe")
ENDIF(WIN32)

#--------------------------------------------------------------------------------
# Install the QtTest application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS ${PROGNAME}
   BUNDLE DESTINATION . COMPONENT Runtime
   RUNTIME DESTINATION bin COMPONENT Runtime
)

#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
IF(WITH_QT4)
	INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
ENDIF()

#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
IF(WITH_QT4)
	INSTALL(CODE "
 	   file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]\nPlugins = plugins\")
 	   " COMPONENT Runtime)
ENDIF()

#--------------------------------------------------------------------------------
# Install needed MC plugins
IF(APPLE)
	INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/segmentation/mc_segmentation.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
	SET(MCPLUGINS "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_segmentation.dylib")
	INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/visualisation/mc_visualisation.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
	SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_visualisation.dylib")

	IF((SUPPORT_IMAGE AND USE_IMAGE) OR (SUPPORT_VIDEO AND USE_VIDEO))
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/image/mc_image.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_image.dylib")
	ENDIF()
	IF(SUPPORT_VIDEO AND USE_VIDEO)
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/video/mc_video.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_video.dylib")
	ENDIF()
	IF(SUPPORT_AUDIO AND USE_AUDIO)
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/audio/mc_audio.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_audio.dylib")
		IF(USE_VAMP)
			INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/vamp/mc_vamp.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
			SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_vamp.dylib")
		ENDIF(USE_VAMP)
	ENDIF()
	IF(SUPPORT_3DMODEL AND USE_3DMODEL)
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/3Dmodel/mc_3Dmodel.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_3Dmodel.dylib")
	ENDIF()
ENDIF()

#--------------------------------------------------------------------------------
# Install needed OSG plugins by copying directories from the OSG plugin installation
IF(APPLE)
	IF(SUPPORT_VIDEO AND USE_VIDEO)
		set(I_OSG_PLUG_VIDEO 0)
		#INSTALL(PROGRAMS "${OSG_FFMPEG_PLUGIN}" DESTINATION ${PROGNAME}.app/Contents/PlugIns COMPONENT Runtime)
		#GET_FILENAME_COMPONENT(OSG_FFMPEG_PLUGIN_NAME ${OSG_FFMPEG_PLUGIN} NAME)
		#SET(OSGPLUGINS "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSG_FFMPEG_PLUGIN_NAME}")
		foreach(OSGPLUGIN_VIDEO ${OSGPLUGINS_VIDEO})
			math(EXPR I_OSG_PLUG_VIDEO ${I_OSG_PLUG_VIDEO}+1)
			INSTALL(PROGRAMS "${OSGPLUGIN_VIDEO}" DESTINATION ${PROGNAME}.app/Contents/PlugIns COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_VIDEO_NAME ${OSGPLUGIN_VIDEO} NAME)
			IF(I_OSG_PLUG_VIDEO EQUAL 1)
				SET(OSGPLUGINS_VIDEO "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_VIDEO_NAME}")
			ELSE()
				SET(OSGPLUGINS_VIDEO "${OSGPLUGINS_VIDEO}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_VIDEO_NAME}")
			ENDIF()
		endforeach(OSGPLUGIN_VIDEO)
		list(LENGTH OSGPLUGINS plugs)
		IF (I_OSG_PLUG_VIDEO GREATER 0)
			IF (plugs EQUAL 0)
				SET(OSGPLUGINS "${OSGPLUGINS_VIDEO}")
			ELSE()
				SET(OSGPLUGINS "${OSGPLUGINS}" "${OSGPLUGINS_VIDEO}")
			ENDIF()
		ENDIF()
	ENDIF()
	IF(SUPPORT_IMAGE AND USE_IMAGE)
		set(I_OSG_PLUG_IMAGE 0)
		foreach(OSGPLUGIN_IMAGE ${OSGPLUGINS_IMAGE})
			math(EXPR I_OSG_PLUG_IMAGE ${I_OSG_PLUG_IMAGE}+1)
			INSTALL(PROGRAMS "${OSGPLUGIN_IMAGE}" DESTINATION ${PROGNAME}.app/Contents/PlugIns COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_IMAGE_NAME ${OSGPLUGIN_IMAGE} NAME)
			IF(I_OSG_PLUG_IMAGE EQUAL 1)
				SET(OSGPLUGINS_IMAGE "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_IMAGE_NAME}")
			ELSE()
				SET(OSGPLUGINS_IMAGE "${OSGPLUGINS_IMAGE}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_IMAGE_NAME}")
			ENDIF()
		endforeach(OSGPLUGIN_IMAGE)
		list(LENGTH OSGPLUGINS plugs)
		IF (I_OSG_PLUG_IMAGE GREATER 0)
			IF (plugs EQUAL 0)
				SET(OSGPLUGINS "${OSGPLUGINS_IMAGE}")
			ELSE()
				SET(OSGPLUGINS "${OSGPLUGINS}" "${OSGPLUGINS_IMAGE}")
			ENDIF()
		ENDIF()
	ENDIF()
	IF(SUPPORT_3DMODEL AND USE_3DMODEL)
		set(I_OSG_PLUG_3DMODEL 0)
		foreach(OSGPLUGIN_3DMODEL ${OSGPLUGINS_3DMODEL})
			math(EXPR I_OSG_PLUG_3DMODEL ${I_OSG_PLUG_3DMODEL}+1)
			INSTALL(PROGRAMS "${OSGPLUGIN_3DMODEL}" DESTINATION ${PROGNAME}.app/Contents/PlugIns COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_3DMODEL_NAME ${OSGPLUGIN_3DMODEL} NAME)
			IF(I_OSG_PLUG_3DMODEL EQUAL 1)
				SET(OSGPLUGINS_3DMODEL "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_3DMODEL_NAME}")
			ELSE()
				SET(OSGPLUGINS_3DMODEL "${OSGPLUGINS_3DMODEL}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_3DMODEL_NAME}")
			ENDIF()
		endforeach(OSGPLUGIN_3DMODEL)
		list(LENGTH OSGPLUGINS plugs)
		IF (I_OSG_PLUG_3DMODEL GREATER 0)
			IF (plugs EQUAL 0)
				SET(OSGPLUGINS "${OSGPLUGINS_3DMODEL}")
			ELSE()
				SET(OSGPLUGINS "${OSGPLUGINS}" "${OSGPLUGINS_3DMODEL}")
			ENDIF()
		ENDIF()
	ENDIF()
	#IF(SUPPORT_TEXT AND USE_TEXT)
		set(I_OSG_PLUG_TEXT 0)
		foreach(OSGPLUGIN_TEXT ${OSGPLUGINS_TEXT})
			math(EXPR I_OSG_PLUG_TEXT ${I_OSG_PLUG_TEXT}+1)
			INSTALL(PROGRAMS "${OSGPLUGIN_TEXT}" DESTINATION ${PROGNAME}.app/Contents/PlugIns COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_TEXT_NAME ${OSGPLUGIN_TEXT} NAME)
			IF(I_OSG_PLUG_TEXT EQUAL 1)
				SET(OSGPLUGINS_TEXT "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_TEXT_NAME}")
			ELSE()
				SET(OSGPLUGINS_TEXT "${OSGPLUGINS_TEXT}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/${OSGPLUGIN_TEXT_NAME}")
			ENDIF()
		endforeach(OSGPLUGIN_TEXT)
		list(LENGTH OSGPLUGINS plugs)
		IF (I_OSG_PLUG_TEXT GREATER 0)
			IF (plugs EQUAL 0)
				SET(OSGPLUGINS "${OSGPLUGINS_TEXT}")
			ELSE()
				SET(OSGPLUGINS "${OSGPLUGINS}" "${OSGPLUGINS_TEXT}")
			ENDIF()
		ENDIF()
	#ENDIF()

ENDIF()
#--------------------------------------------------------------------------------
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# Now the work of copying dependencies into the bundle/package
# The quotes are escaped and variables to use at install time have their $ escaped
# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# over.
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    #fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${LINKED_DIRECTORIES}\")
    fixup_bundle(\"${APPS}\" \"${MCPLUGINS};${OSGPLUGINS}\" \"${LINKED_DIRECTORIES}\")
    " COMPONENT Runtime)

# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# where CPackConfig.cmake is created by including CPack
# And then there's ways to customize this as well
set(CPACK_PACKAGE_NAME "${PROGNAME}")
set(CPACK_BUNDLE_NAME "${PROGNAME}")
set(CPACK_BINARY_DRAGNDROP ON)
set(CPACK_PACKAGE_EXECUTABLES "multimediacycle-osg-qt" "MultiMediaCycle.icns") #should contain pairs of <executable> and <icon name>
#set(CPACK_GENERATOR "PackageMaker;OSXX11")
#include(CPack)