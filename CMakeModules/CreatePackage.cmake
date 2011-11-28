# MediaCycle packaging system.
#
# at the end of any application that needs packaging, adapt the following variables:
# * SET(USE_AUDIO ON), SET(USE_IMAGE ON), SET(USE_VIDEO ON), SET(USE_3DMODEL ON), SET(USE_TEXT ON) to match the supported media types,
#   it optimizes the bundled libraries for single-media applications
# * SET(PROGNAME "app-name") where app-name matches the CMake target name of the application
# * (optional) SET(WITH_QT4 ON) if you application uses Qt4 as GUI framework
# * (optional) SET(MC_PACKAGE_DESCRIPTION "...")
# then add after: INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
#
# to create the package (with app-name the CMake target name of the application):
# Apple *.app bundle: make app-name install
# Linux *.deb package: cpack --config CPackConfig-app-name.cmake

# PACKAGING WITH CPack
# For more see http://www.cmake.org/Wiki/CMake:Packaging_With_CPack
IF(NOT USE_DEBUG) # mandatory for packaging release versions

IF(UNIX OR APPLE) # not yet tested with Windows 
	INCLUDE(InstallRequiredSystemLibraries)
	set(CPACK_PACKAGE_NAME "${PROGNAME}")
	set(CPACK_BUNDLE_NAME "${PROGNAME}")
    IF(NOT CPACK_PACKAGE_DESCRIPTION_SUMMARY AND NOT MC_PACKAGE_DESCRIPTION_SUMMARY)
	    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Software part of the MediaCycle framework")
    ELSE()
	    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${MC_PACKAGE_DESCRIPTION_SUMMARY}")   
    ENDIF()
    IF(NOT CPACK_PACKAGE_DESCRIPTION AND NOT MC_PACKAGE_DESCRIPTION)
	    set(CPACK_PACKAGE_DESCRIPTION "MediaCycle is a framework for the navigation in multimedia content databases by similarity, using a content-based approach fueled by several feature extraction and clustering algorithms. For more information check http://www.numediart.org")
    ELSE()
	    set(CPACK_PACKAGE_DESCRIPTION "${MC_PACKAGE_DESCRIPTION}")   
    ENDIF()
	SET(CPACK_PACKAGE_VENDOR "numediart")
	IF(NOT CPACK_PACKAGE_CONTACT)
        set(CPACK_PACKAGE_CONTACT "http://numediart.org/")
    ENDIF()
	IF(NOT CPACK_PACKAGE_VERSION)
        set(CPACK_PACKAGE_VERSION "${MediaCycle_VERSION}")
    ENDIF()
	set(CPACK_SOURCE_IGNORE_FILES
		"^${PROJECT_SOURCE_DIR}/Builds/"
	)
    #Unused so far, since we make single application packages instead of a single framework distribution
	#set(CPACK_PACKAGE_EXECUTABLES "multimediacycle-osg-qt" "MultiMediaCycle.icns") #should contain pairs of <executable> and <icon name>
	IF (APPLE)
		#set(CPACK_BUNDLE_NAME "MC")
		set(CPACK_GENERATOR "DragNDrop")#to test: set(CPACK_GENERATOR "PackageMaker;OSXX11")
	ELSE()
		set(CPACK_GENERATOR "TBZ2")
	ENDIF()
ENDIF ( UNIX OR APPLE )

# Borrowed from Performous performous-packaging.cmake CMakeModule
IF(UNIX)
	# Try to find architecture
	execute_process(COMMAND uname -m OUTPUT_VARIABLE CPACK_PACKAGE_ARCHITECTURE)
	string(STRIP "${CPACK_PACKAGE_ARCHITECTURE}" CPACK_PACKAGE_ARCHITECTURE)
	# Try to find distro name and distro-specific arch
	execute_process(COMMAND lsb_release -is OUTPUT_VARIABLE LSB_ID)
	execute_process(COMMAND lsb_release -rs OUTPUT_VARIABLE LSB_RELEASE)
	string(STRIP "${LSB_ID}" LSB_ID)
	string(STRIP "${LSB_RELEASE}" LSB_RELEASE)
	set(LSB_DISTRIB "${LSB_ID}${LSB_RELEASE}")
	IF(NOT LSB_DISTRIB)
		set(LSB_DISTRIB "unix")
	ENDIF(NOT LSB_DISTRIB)

	IF(NOT APPLE)
		SET(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackConfig-${PROGNAME}.cmake")
		SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};MediaCycle;${PROGNAME};/")
		#EXECUTE_PROCESS(COMMAND rm "${CMAKE_BINARY_DIR}/CPackConfig.cmake")
	ENDIF()

	# For Debian-based distros we want to create DEB packages.
	IF("${LSB_DISTRIB}" MATCHES "Ubuntu|Debian")
	    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "numediart.org")
		set(CPACK_GENERATOR "DEB")
		set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
		set(CPACK_DEBIAN_PACKAGE_SECTION "universe/multimedia")
		#set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "..., ...")
		SET(CPACK_DEBIAN_PACKAGE_VERSION "${MediaCycle_VERSION}")
		SET(CPACK_DEBIAN_PACKAGE_NAME "${PROGNAME}")

		# We need to alter the architecture names as per distro rules
		IF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "i[3-6]86")
			set(CPACK_PACKAGE_ARCHITECTURE i386)
		ENDIF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "i[3-6]86")
		IF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "x86_64")
			set(CPACK_PACKAGE_ARCHITECTURE amd64)
		ENDIF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "x86_64")

		# Set the dependencies based on the distro version

		# Ubuntu
        IF("${LSB_DISTRIB}" MATCHES "Ubuntu")
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER "numediart")
        ENDIF()

	IF("${LSB_DISTRIB}" MATCHES "Ubuntu1") # for our interest, 10.04, 10.10, 11.04

            # TinyXML
            # for previous versions 3rdParty/tinyxml is used		
            IF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.04"))
                SET(UBUNTU_DEPS "libtinyxml2.5.3")
            ELSEIF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu12.04"))
                SET(UBUNTU_DEPS "libtinyxml2.6.2")
            ENDIF()
	
            # Boost
            IF("${LSB_DISTRIB}" MATCHES "Ubuntu10.04")
                SET(UBUNTU_DEPS "libboost-serialization1.40.0" "libboost-system1.40.0" "libboost-filesystem1.40.0" "libboost-graph1.40.0" "libboost-thread1.40.0")
            ELSEIF("${LSB_DISTRIB}" MATCHES "Ubuntu10.10")
                SET(UBUNTU_DEPS "libboost-serialization1.40.0|libboost-serialization1.42.0" "libboost-system1.40.0|libboost-system1.42.0" "libboost-filesystem1.40.0|libboost-filesystem1.42.0" "libboost-graph1.40.0|libboost-graph1.42.0" "libboost-thread1.40.0|libboost-thread1.42.0")
            ELSEIF("${LSB_DISTRIB}" MATCHES "Ubuntu11.04")
                SET(UBUNTU_DEPS "libboost-serialization1.42.0" "libboost-system1.42.0" "libboost-filesystem1.42.0" "libboost-graph1.42.0" "libboost-thread1.42.0")
            ENDIF()

            # Armadillo
            IF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu12.04"))
                list(APPEND UBUNTU_DEPS "libatlas3gf-base" "liblapack3gf" "libblas3gf" "libarmadillo2")
            ELSEIF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.04"))
                list(APPEND UBUNTU_DEPS "libatlas3gf-base" "liblapack3gf" "libblas3gf" "libarmadillo0")
            ENDIF()

            # OpenSceneGraph
            IF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu12.04"))
                list(APPEND UBUNTU_DEPS "libopenscenegraph80" "libopenthreads14")
            ELSEIF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.04"))
                list(APPEND UBUNTU_DEPS "libopenscenegraph65" "libopenthreads13")
            ENDIF()

            IF(WITH_QT4 AND SUPPORT_VIDEO AND USE_VIDEO AND FFMPEG_FOUND) # dirty test to check if we're packaging a GUI application under Ubuntu
                #INSTALL(FILES ${CMAKE_BINARY_DIR}/3rdparty/osgdb_ffmpeg/osgdb_ffmpeg.so DESTINATION lib/osgPlugins-${OPENSCENEGRAPH_VERSION} COMPONENT ${PROGNAME})
            ENDIF()

            # Qt4 libqtcore4 and libqtgui4 are package for kubuntu, libqt4-core and libqt4-gui for ubuntu
            IF(USE_QT4 AND WITH_QT4)
                list(APPEND UBUNTU_DEPS "libqt4-core|libqtcore4" "libqt4-gui|libqtgui4" "libqt4-opengl" "libqt4-svg" "libqt4-xml")
                IF(QT_USE_PHONON)
                    list(APPEND UBUNTU_DEPS "libphonon4")
                ENDIF()
            ENDIF()
 
            IF(SUPPORT_AUDIO)

                # sndfile 
               list(APPEND UBUNTU_DEPS "libsndfile1")

                # OpenAL
                IF(USE_OPENAL)
                    list(APPEND UBUNTU_DEPS "libopenal1") 
                ENDIF()

                # PortAudio (+ libportaudiocpp0?)
                IF(USE_PORTAUDIO)
                    list(APPEND UBUNTU_DEPS "libportaudio0|libportaudio2") # note that portaudio2 may have better jack support
                ENDIF()

                # samplerate
                IF(USE_AUDIOFEAT)
                    list(APPEND UBUNTU_DEPS "libsamplerate0")
                ENDIF()
            ENDIF()

            # FFmpeg
            IF(SUPPORT_VIDEO)
                list(APPEND UBUNTU_DEPS "libavcodec-dev" "libavdevice-dev" "libavformat-dev" "libavutil-dev" "libswscale-dev" "libpostproc-dev")#depending on -dev packages so in case multiple lib versions are installed
            ENDIF()

            IF(SUPPORT_VIDEO OR SUPPORT_IMAGE)

                # OpenCV
		IF(OpenCV_VERSION VERSION_GREATER 2.3.0)
                    IF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.10"))
                    	#MESSAGE("\n\nWARNING!\nRepackage an up-to-date OpenCV 2.3.1 package from https://launchpad.net/~gijzelaar/+archive/opencv2.3 against recent FFmpeg > 0.8.x packages from https://launchpad.net/~jon-severinsson/+archive/ffmpeg") # no we need to repackage them against more uptodate ffmpeg packages rebuilt from Jon Severinsson's ppa source packages
                    	list(APPEND UBUNTU_DEPS "libopencv-dev")
                    ELSE()
                    	MESSAGE(FATAL_ERROR "OpenCV >= 2.3.0 not available as package for your distribution")
                    ENDIF()
		ELSE()	
                    IF("${LSB_DISTRIB}" MATCHES "Ubuntu10.04")
                        list(APPEND UBUNTU_DEPS "libcv4" "libcvaux4" "libhighgui4")
                    ELSE()
                        list(APPEND UBUNTU_DEPS "libcv2.1" "libcvaux2.1" "libhighgui2.1")
                    ENDIF()
                ENDIF()

                # FFTW3
                list(APPEND UBUNTU_DEPS "libfftw3-3")
            ENDIF()

            # Mat File I/O
            #IF(MATIO_FOUND)
            #    list(APPEND UBUNTU_DEPS "libmatio0")
            #ENDIF()

            # StarPU, not yet packaged
            #IF(STARPU_FOUND)
            #    list(APPEND UBUNTU_DEPS "libstarpu")
            #ENDIF()

            # Torch3
            IF(TORCH3_FOUND)
                list(APPEND UBUNTU_DEPS "libtorch3c2")
            ENDIF()

	    # liblo
            IF(USE_OSC AND LIBLO_FOUND)
                list(APPEND UBUNTU_DEPS "liblo7")
            ENDIF()

            # clucene
            IF(CLUCENE_FOUND AND SUPPORT_TEXT AND USE_TEXTFEAT)
                list(APPEND UBUNTU_DEPS "libclucene0ldbl")
            ENDIF()

            # PoDoFo
            IF(PODOFO_FOUND AND SUPPORT_PDF)
                IF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.04"))
                    list(APPEND UBUNTU_DEPS "libpodofo0.8.0")
                ELSEIF(("${LSB_DISTRIB}" MATCHES "Ubuntu10.04") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu11.10") OR ("${LSB_DISTRIB}" MATCHES "Ubuntu12.04"))
                    list(APPEND UBUNTU_DEPS "libpodofo0.9.0")
                ENDIF()
            ENDIF()

            STRING(REGEX REPLACE ";" ", " UBUNTU_DEPS "${UBUNTU_DEPS}")
            #MESSAGE("Ubuntu 10.10 deps: ${UBUNTU_DEPS}")
			set(CPACK_DEBIAN_PACKAGE_DEPENDS ${UBUNTU_DEPS})
		ENDIF()

		# Debian
		# IF("${LSB_DISTRIB}" MATCHES "Debian5.*")
		#   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian, libcairo2, librsvg2-2, libboost-dev, libavcodec51, libavformat52, libswscale0, libmagick++10, libxml++2.6-2, libglew1.5")
		# ENDIF("${LSB_DISTRIB}" MATCHES "Debian5.*")

		# IF("${LSB_DISTRIB}" MATCHES "Debiantesting")
        	#   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian, libcairo2, librsvg2-2, libboost-dev, libavcodec52, libavformat52, libswscale0, libmagick++3, libxml++2.6-2, libglew1.5")
        	# ENDIF("${LSB_DISTRIB}" MATCHES "Debiantesting")

		IF(NOT CPACK_DEBIAN_PACKAGE_DEPENDS)
			message("WARNING: ${LSB_DISTRIB} not supported yet.\nPlease set deps in CMakeModules/CreatePackage.cmake before packaging.")
		ENDIF(NOT CPACK_DEBIAN_PACKAGE_DEPENDS)
		string(TOLOWER "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-${LSB_DISTRIB}_${CPACK_PACKAGE_ARCHITECTURE}" CPACK_PACKAGE_FILE_NAME)

		# Install the icon file
		INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/icons/48px/MultiMediaCycle.png DESTINATION share/pixmaps COMPONENT ${PROGNAME} RENAME ${PROGNAME}.png)
		#INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/icons/32px/MultiMediaCycle.xpm DESTINATION share/pixmaps COMPONENT ${PROGNAME} RENAME ${PROGNAME}.xpm)

		# Install the .desktop description
		file(WRITE ${CMAKE_BINARY_DIR}/${PROGNAME}.desktop [Desktop\ Entry]\nType=Application\nExec=${PROGNAME}\nMimeType=application/x-${PROGNAME};\nIcon=${PROGNAME}\nName=${PROGNAME}\nGenericName=${MC_PACKAGE_DESCRIPTION_SUMMARY}\nComment=${MC_PACKAGE_DESCRIPTION_SUMMARY})
		INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROGNAME}.desktop DESTINATION share/applications COMPONENT ${PROGNAME})

	ENDIF("${LSB_DISTRIB}" MATCHES "Ubuntu|Debian")
	# For Fedora-based distros we want to create RPM packages.
	# IF("${LSB_DISTRIB}" MATCHES "Fedora")
	# 	set(CPACK_GENERATOR "RPM")
	# 	set(CPACK_RPM_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
	# 	set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION}")
	# 	set(CPACK_RPM_PACKAGE_RELEASE "1")
	# 	set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
	# 	set(CPACK_RPM_PACKAGE_LICENSE "LGPL?")
	# 	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MediaCycle, a framework for navigation by similarity in multimedia databases.")
	# 	set(CPACK_RPM_PACKAGE_DESCRIPTION "MediaCycle...")
		# We need to alter the architecture names as per distro rules
	# 	IF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "i[3-6]86")
	# 		set(CPACK_PACKAGE_ARCHITECTURE i386)
	# 	ENDIF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "i[3-6]86")
	# 	IF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "x86_64")
	# 		set(CPACK_PACKAGE_ARCHITECTURE amd64)
	# 	ENDIF("${CPACK_PACKAGE_ARCHITECTURE}" MATCHES "x86_64")
	# 	# Set the dependencies based on the distro version
	# 	IF("${LSB_DISTRIB}" MATCHES "Fedora14")
	# 		set(CPACK_RPM_PACKAGE_REQUIRES "..., ...")
	# 	ENDIF("${LSB_DISTRIB}" MATCHES "Fedora14")
	# 	IF("${LSB_DISTRIB}" MATCHES "Fedora13")
	# 		set(CPACK_RPM_PACKAGE_REQUIRES "..., ...")
	# 	ENDIF("${LSB_DISTRIB}" MATCHES "Fedora13")
	# 	IF(NOT CPACK_RPM_PACKAGE_REQUIRES)
	# 		message("WARNING: ${LSB_DISTRIB} is not supported.\nPlease set deps in cmake/performous-packaging.cmake before packaging.")
	# 	ENDIF(NOT CPACK_RPM_PACKAGE_REQUIRES)
	#  ENDIF("${LSB_DISTRIB}" MATCHES "Fedora")
	set(CPACK_SYSTEM_NAME "${LSB_DISTRIB}-${CPACK_PACKAGE_ARCHITECTURE}")
ENDIF(UNIX)

#From: http://www.cmake.org/Wiki/BundleUtilitiesExample
SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(qtframeworks_dest_dir bin)
SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGNAME}")
IF(APPLE)
  SET(plugin_dest_dir ${PROGNAME}.app/Contents/MacOS)
  SET(qtconf_dest_dir ${PROGNAME}.app/Contents/Resources)
  SET(qtframeworks_dest_dir ${PROGNAME}.app/Contents/Resources)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app")
ENDIF(APPLE)
IF(WIN32)
  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${PROGNAME}.exe")
ENDIF(WIN32)

#--------------------------------------------------------------------------------
# Install the QtTest application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
INSTALL(TARGETS ${PROGNAME}
   BUNDLE DESTINATION . COMPONENT ${PROGNAME}
   RUNTIME DESTINATION bin COMPONENT ${PROGNAME}
)

# Install the plugins for Linux (and Windows?)
IF(NOT APPLE)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/visualisation/${PLUGIN_PREFIX}visualisation.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/segmentation/${PLUGIN_PREFIX}segmentation.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    IF(SUPPORT_AUDIO AND USE_AUDIO)
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/audio/${PLUGIN_PREFIX}audio.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
    IF(SUPPORT_IMAGE AND USE_IMAGE)
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/image/${PLUGIN_PREFIX}image.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
    IF(SUPPORT_VIDEO AND USE_VIDEO)
	INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/image/${PLUGIN_PREFIX}image.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/video/${PLUGIN_PREFIX}video.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
    IF(SUPPORT_3DMODEL AND USE_3DMODEL)
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/3Dmodel/${PLUGIN_PREFIX}3Dmodel.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
    IF(SUPPORT_TEXT AND USE_TEXT)
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/SparseVisualisation/${PLUGIN_PREFIX}SparseVisualisation.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/text/${PLUGIN_PREFIX}text.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/text_sparse/${PLUGIN_PREFIX}text_sparse.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
    IF(SUPPORT_ARCHIPEL AND USE_ARCHIPEL)
        INSTALL(FILES ${CMAKE_BINARY_DIR}/plugins/archipel/${PLUGIN_PREFIX}archipel.${PLUGIN_SUFFIX} DESTINATION lib COMPONENT ${PROGNAME})
    ENDIF()
ENDIF()

#--------------------------------------------------------------------------------
# Install needed Qt plugins by copying directories from the qt installation
# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
IF(APPLE)
IF(WITH_QT4)
	INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
ENDIF()
ENDIF()
#--------------------------------------------------------------------------------
# install a qt.conf file
# this inserts some cmake code into the install script to write the file
IF(APPLE)
IF(WITH_QT4)
	INSTALL(CODE "
 	   file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]\nPlugins = plugins\")
 	   " COMPONENT Runtime)
	# Really ugly, supposing Qt is installed thru MacPorts on Apple
	INSTALL(DIRECTORY "/opt/local/lib/Resources/qt_menu.nib" DESTINATION ${qtframeworks_dest_dir} COMPONENT Runtime)
ENDIF()
ENDIF()

#--------------------------------------------------------------------------------
# Install needed MC plugins
IF(APPLE)#to generalize for other platforms
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
	IF(SUPPORT_TEXT AND USE_TEXT)
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/text/mc_text.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_text.dylib")
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/text_sparse/mc_text_sparse.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_text_sparse.dylib")
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/SparseVisualisation/mc_SparseVisualisation.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_SparseVisualisation.dylib")
	ENDIF()
	IF(SUPPORT_ARCHIPEL AND USE_ARCHIPEL)
		INSTALL(PROGRAMS "${CMAKE_BINARY_DIR}/plugins/archipel/mc_archipel.dylib" DESTINATION ${PROGNAME}.app/Contents/MacOS COMPONENT Runtime)
		SET(MCPLUGINS "${MCPLUGINS}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/MacOS/mc_archipel.dylib")
	ENDIF()
ENDIF()

#--------------------------------------------------------------------------------
# Install needed OSG plugins by copying directories from the OSG plugin installation
IF(APPLE)
	IF(SUPPORT_VIDEO AND USE_VIDEO)
		set(I_OSG_PLUG_VIDEO 0)
		foreach(OSGPLUGIN_VIDEO ${OSGPLUGINS_VIDEO})
			math(EXPR I_OSG_PLUG_VIDEO ${I_OSG_PLUG_VIDEO}+1)
			INSTALL(PROGRAMS "${OSGPLUGIN_VIDEO}" DESTINATION ${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION} COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_VIDEO_NAME ${OSGPLUGIN_VIDEO} NAME)
			IF(I_OSG_PLUG_VIDEO EQUAL 1)
				SET(OSGPLUGINS_VIDEO "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_VIDEO_NAME}")
			ELSE()
				SET(OSGPLUGINS_VIDEO "${OSGPLUGINS_VIDEO}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_VIDEO_NAME}")
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
			INSTALL(PROGRAMS "${OSGPLUGIN_IMAGE}" DESTINATION ${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION} COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_IMAGE_NAME ${OSGPLUGIN_IMAGE} NAME)
			IF(I_OSG_PLUG_IMAGE EQUAL 1)
				SET(OSGPLUGINS_IMAGE "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_IMAGE_NAME}")
			ELSE()
				SET(OSGPLUGINS_IMAGE "${OSGPLUGINS_IMAGE}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_IMAGE_NAME}")
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
			INSTALL(PROGRAMS "${OSGPLUGIN_3DMODEL}" DESTINATION ${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION} COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_3DMODEL_NAME ${OSGPLUGIN_3DMODEL} NAME)
			IF(I_OSG_PLUG_3DMODEL EQUAL 1)
				SET(OSGPLUGINS_3DMODEL "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_3DMODEL_NAME}")
			ELSE()
				SET(OSGPLUGINS_3DMODEL "${OSGPLUGINS_3DMODEL}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_3DMODEL_NAME}")
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
			INSTALL(PROGRAMS "${OSGPLUGIN_TEXT}" DESTINATION ${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION} COMPONENT Runtime)
			GET_FILENAME_COMPONENT(OSGPLUGIN_TEXT_NAME ${OSGPLUGIN_TEXT} NAME)
			IF(I_OSG_PLUG_TEXT EQUAL 1)
				SET(OSGPLUGINS_TEXT "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_TEXT_NAME}")
			ELSE()
				SET(OSGPLUGINS_TEXT "${OSGPLUGINS_TEXT}" "${CMAKE_INSTALL_PREFIX}/${PROGNAME}.app/Contents/PlugIns/osgPlugins-${OPENSCENEGRAPH_VERSION}/${OSGPLUGIN_TEXT_NAME}")
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
IF(APPLE)
INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    #fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${LINKED_DIRECTORIES}\")
    fixup_bundle(\"${APPS}\" \"${MCPLUGINS};${OSGPLUGINS};${QTPLUGINS}\" \"${LINKED_DIRECTORIES}\")
    " COMPONENT Runtime)
ENDIF()

IF(APPLE)
	# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
	# where CPackConfig.cmake is created by including CPack
	# And then there's ways to customize this as well
	set(CPACK_PACKAGE_NAME "${PROGNAME}")
	set(CPACK_BUNDLE_NAME "${PROGNAME}")
	set(CPACK_BINARY_DRAGNDROP ON)
	#set(CPACK_PACKAGE_EXECUTABLES "multimediacycle-osg-qt" "MultiMediaCycle.icns") #should contain pairs of <executable> and <icon name>
	#set(CPACK_GENERATOR "PackageMaker;OSXX11")
ENDIF()

IF(NOT APPLE)
	include(CPack)
	#EXECUTE_PROCESS(COMMAND rm "${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake")
ENDIF()

ENDIF(NOT USE_DEBUG) # mandatory for packaging release versions
