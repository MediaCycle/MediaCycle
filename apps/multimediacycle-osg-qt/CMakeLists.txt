SET(TARGET_NAME "MultiMediaCycle")
IF(BUILD_MULTIMEDIACYCLE_APP AND USE_QT4 AND USE_OSG AND USE_OSC )

        # Media configurations that currently don't have specific apps
        INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreateDefaultConfig.cmake)
	# 3DmodelCycle
        SET(DESCRIPTION "3DmodelCycle allows to organize 3D models by similarity.")
        SET(MEDIA_TYPE "MEDIA_TYPE_3DMODEL")
        SET(MC_PLUGINS "3dmodel;file;pcapreprocess;visualisation")
        SET(OSG_PLUGINS "osgdb_osg")
        CREATE_DEFAULT_CONFIG("3DmodelCycle")
	# MakamCycle
        IF(SUPPORT_AUDIO AND OCTAVE_FOUND AND USE_OCTAVE AND USE_MAKAM)
	    SET(DESCRIPTION "MakamCycle attempts to help understand Makam music.")
            SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
            SET(MC_PLUGINS "audio-engine;audio-features-yaafe;audio-makam;audio-osg;audio-reader-sndfile;file;pcapreprocess;visualisation")
            SET(OSG_PLUGINS "")
            CREATE_DEFAULT_CONFIG("MakamCycle")
        ENDIF()

        SET(DESCRIPTION "MultiMediaCycle allows to visually organize media files by similarity.")

	FILE(GLOB PLUGIN_DIRS ${CMAKE_SOURCE_DIR}/plugins/*)
	foreach(PLUGIN_DIR ${PLUGIN_DIRS})
		GET_FILENAME_COMPONENT(PLUGIN_NAME ${PLUGIN_DIR} NAME_WE)
		#MESSAGE("Adding plugin: '${PLUGIN_NAME}'")
		IF(MC_PLUGINS)
			SET(MC_PLUGINS "${MC_PLUGINS}" "${PLUGIN_NAME}")
		ELSE()
			SET(MC_PLUGINS "${PLUGIN_NAME}")
		ENDIF()
	endforeach(PLUGIN_DIR)
        #MESSAGE("List of plugins: '${MC_PLUGINS}'")

        FILE(GLOB DEFAULT_CONFIGS ${CMAKE_BINARY_DIR}/apps/*-osg-qt/AC*DefaultConfig*.h)
	
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

	INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/AddMediaCycleOsgQtApp.cmake)
	ADD_MC_OSG_QT_EXECUTABLE(${TARGET_NAME})

	# Add more optional dependencies:
	#ADD_DEPENDENCIES(${TARGET_NAME} É)
	# Link these dependences:
	#TARGET_LINK_LIBRARIES(${TARGET_NAME} É)
	
	# Packaging
	SET(PROGNAME "${TARGET_NAME}")
	IF(NOT USE_DEBUG AND (APPLE OR UNIX))
		INCLUDE (${CMAKE_SOURCE_DIR}/CMakeModules/CreatePackage.cmake)
	ENDIF()

	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
