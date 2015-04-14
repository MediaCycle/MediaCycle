#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2012 – UMONS - Numediart
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

# Requires the following variables to be set:
#  MEDIA_TYPE, example: SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
#  DESCRIPTION: example: SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
#  MC_PLUGINS (list): example SET(MC_PLUGINS "segmentation;visualisation")
#
# Required (osg)
#  OSG_PLUGINS (list): example SET(OSG_PLUGINS "osgdb_ffmpeg;osgdb_svg")
#
# Optional
#  MC_PLUGINS_STATIC: example SET(MC_PLUGINS_STATIC ON)
#  BROWSER_MODE: example: SET(BROWSER_MODE "AC_MODE_CLUSTERS"), otherwise defaults to clusters mode
#  CLUSTERS_METHOD: example: SET(CLUSTERS_METHOD "MediaCycle KMeans")
#  CLUSTERS_POSITIONS: example: SET(CLUSTERS_POSITIONS "MediaCycle Propeller")
#  NEIGHBORS_METHOD: example: SET(NEIGHBORS_METHOD "Pareto")
#  NEIGHBORS_POSITIONS: example: SET(NEIGHBORS_POSITIONS "Radial Tree")
#  PREPROCESS: example: SET(PREPROCESS "PCA Preprocess")
#  SEGMENTATION: example: SET(SEGMENTATION "BIC")
#  FILTERING: example: SET(FILTERING "Proximity Grid")
#  WITH_SEGMENTATION: example: SET(WITH_SEGMENTATION ON)
#  WITHOUT_NEIGHBORS: example: SET(WITHOUT_NEIGHBORS ON)
#  MEDIA_READER: example: SET(MEDIA_READER "Archipel Reader")
#  ACTIVE_MEDIA: example: SET(ACTIVE_MEDIA "text")
#  LIBRARY_READER: example: SET(LIBRARY_READER "CSV import")
#  LIBRARY_WRITER: example: SET(LIBRARY_WRITER "CSV export")
#
# Optional (osg-qt)
#  WITH_OSC: example SET(WITH_OSC ON)
#  CONNECT_OSC: example SET(CONNECT_OSC ON)
#  FULLSCREEN: example SET(FULLSCREEN ON)
#  SETTING_INSTALLATION: example SET(SETTING_INSTALLATION ON)
#  HIDE_INFORMATION: example SET(HIDE_INFORMATION ON)
#  FORGET_THUMBNAILS: example SET(FORGET_THUMBNAILS ON)

macro(CREATE_DEFAULT_CONFIG TARGET_NAME)

IF(NOT MEDIA_TYPE)
	MESSAGE(FATAL_ERROR "MEDIA_TYPE not set")
ENDIF()
IF(NOT DESCRIPTION)
    SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
ENDIF()
IF(NOT BROWSER_MODE)
        SET(BROWSER_MODE "AC_MODE_CLUSTERS")
        #MESSAGE("BROWSER_MODE not set, defaulting to ${BROWSER_MODE}")
ENDIF()
IF(MC_PLUGINS)
	LIST(REMOVE_DUPLICATES MC_PLUGINS)
ENDIF()
IF(NOT MC_PLUGINS AND NOT MC_PLUGINS_STATIC)
	MESSAGE(FATAL_ERROR "No plugin set, list in MC_PLUGINS")
ENDIF()

STRING(REGEX REPLACE " " "" ROOT_NAME "${TARGET_NAME}")

SET(BASE_CLASS "ACAbstractDefaultConfig")
SET(CLASS_NAME "AC${ROOT_NAME}DefaultConfig")
IF(FULLSCREEN OR OSG_PLUGINS)
	SET(WITH_OSG ON)
	SET(BASE_CLASS "ACOsgAbstractDefaultConfig")
	SET(CLASS_NAME "ACOsg${ROOT_NAME}DefaultConfig")
	IF(OSG_PLUGINS)
		SET(BASE_CLASS "ACOsgAbstractDefaultConfig")
		SET(CLASS_NAME "ACOsg${ROOT_NAME}DefaultConfig")
	ENDIF()
ENDIF()

FILE(GLOB PREV_CONFIGS ${CMAKE_CURRENT_BINARY_DIR}/*${ROOT_NAME}*h)
foreach(PREV_CONFIG ${PREV_CONFIGS})
	IF(EXISTS ${PREV_CONFIG})
		execute_process(COMMAND "rm" "${PREV_CONFIG}")
	ENDIF()
endforeach(PREV_CONFIG)


file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
/*
 *  ${CLASS_NAME}.h
 *  MediaCycle
 *
 *  Don't modify this file, rather its generator:
 *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
 *
 */

#ifndef __${CLASS_NAME}_H__
#define __${CLASS_NAME}_H__


#ifdef MC_PLUGINS_STATIC
")

SET(MC_PLUGIN_LIBRARIES)
SET(MC_PLUGIN_FACTORIES)
FOREACH(PLUGIN_NAME ${MC_PLUGINS})
    IF(MC_PLUGINS_STATIC)
        IF(NOT TARGET ${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin)
            MESSAGE(FATAL_ERROR "Plugin '${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin' doesn't exist in ${CMAKE_SOURCE_DIR}/plugins/${PLUGIN_NAME} (case sensitive)")
        ENDIF()
    ELSE()
        IF(NOT TARGET ${PLUGIN_PREFIX}${PLUGIN_NAME})
            MESSAGE(FATAL_ERROR "Plugin '${PLUGIN_PREFIX}${PLUGIN_NAME}' doesn't exist in ${CMAKE_SOURCE_DIR}/plugins/${PLUGIN_NAME} (case sensitive)")
        ENDIF()
    ENDIF()
    SET(PLUGIN_LIBRARY_NAME "AC")
    SET(PLUGIN_WORDS ${PLUGIN_NAME})
    STRING(REGEX REPLACE "-" ";" PLUGIN_WORDS ${PLUGIN_WORDS})
    FOREACH(PLUGIN_WORD ${PLUGIN_WORDS})
        STRING(SUBSTRING ${PLUGIN_WORD} 0 1 FIRST_LETTER)
        STRING(TOUPPER ${FIRST_LETTER} FIRST_LETTER)
        STRING(REGEX REPLACE "^.(.*)" "${FIRST_LETTER}\\1" PLUGIN_WORD_CAP "${PLUGIN_WORD}")
        SET(PLUGIN_LIBRARY_NAME "${PLUGIN_LIBRARY_NAME}${PLUGIN_WORD_CAP}")
    ENDFOREACH(PLUGIN_WORD)
    SET(PLUGIN_LIBRARY_NAME "${PLUGIN_LIBRARY_NAME}PluginLibrary")
    #MESSAGE("PLUGIN_LIBRARY_NAME: ${PLUGIN_LIBRARY_NAME}")

    #IF(EXISTS ${CMAKE_BINARY_DIR}/plugins/mc_${MC_PLUGINS}/${PLUGIN_LIBRARY_NAME}.h)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "#include<${PLUGIN_LIBRARY_NAME}.h>\n")
        LIST(APPEND MC_PLUGIN_LIBRARIES ${PLUGIN_LIBRARY_NAME})
        LIST(APPEND MC_PLUGIN_FACTORIES "if(_name==\"${PLUGIN_NAME}\"){ return new ${PLUGIN_LIBRARY_NAME}()")
    #ENDIF()
ENDFOREACH(PLUGIN_NAME)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "#endif

#include \"${BASE_CLASS}.h\"

class ${CLASS_NAME}: public ${BASE_CLASS} {

public:
    ${CLASS_NAME}() : ${BASE_CLASS}(){}
    virtual ~${CLASS_NAME}(){}
    virtual std::string name(){return \"${TARGET_NAME}\";}
    virtual std::string description(){return \"${DESCRIPTION}\";}
    virtual ACMediaType mediaType(){return ${MEDIA_TYPE};}
    virtual ACBrowserMode browserMode(){return ${BROWSER_MODE};}
")

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
    virtual std::vector<std::string> pluginLibraries(){
        std::vector<std::string> plugins;
")
#IF(NOT MC_PLUGINS_STATIC)
foreach(PLUGIN ${MC_PLUGINS})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\tplugins.push_back(\"${PLUGIN}\");\n")
endforeach(PLUGIN)
#ENDIF()
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\treturn plugins;
    }
")

IF(MC_PLUGINS_STATIC)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool staticLibraries(){return true;}")
ELSE()
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool staticLibraries(){return false;}\n")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
    virtual ACPluginLibrary* createPluginLibrary(std::string _name){
        ACPluginLibrary* library = 0;
        #ifdef MC_PLUGINS_STATIC\n")

    FOREACH(MC_PLUGIN_FACTORY ${MC_PLUGIN_FACTORIES})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t${MC_PLUGIN_FACTORY};}\n")
    ENDFOREACH(MC_PLUGIN_FACTORY)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t#endif\n\treturn library;\n\t}\n")

IF(WITH_OSG)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual std::vector<std::string> osgPlugins(){
		std::vector<std::string> plugins;
")
foreach(OSG_PLUGIN ${OSG_PLUGINS})
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\tplugins.push_back(\"${OSG_PLUGIN}\");\n")
endforeach(OSG_PLUGIN)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\treturn plugins;
    }
")

IF(HIDE_INFORMATION)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool hideInformation(){return true;}\n")
ELSE()
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool hideInformation(){return false;}\n")
ENDIF()

ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
	virtual std::string clustersMethodPlugin(){return \"${CLUSTERS_METHOD}\";}
	virtual std::string clustersPositionsPlugin(){return \"${CLUSTERS_POSITIONS}\";}
	virtual std::string neighborsMethodPlugin(){return \"${NEIGHBORS_METHOD}\";}
	virtual std::string neighborsPositionsPlugin(){return \"${NEIGHBORS_POSITIONS}\";}
	virtual std::string preProcessPlugin(){return \"${PREPROCESS}\";}
	virtual std::string segmentationPlugin(){return \"${SEGMENTATION}\";}
        virtual std::string libraryReaderPlugin(){return \"${LIBRARY_READER}\";}
        virtual std::string libraryWriterPlugin(){return \"${LIBRARY_WRITER}\";}
        virtual std::string filteringPlugin(){return \"${FILTERING}\";}
")

IF(FORGET_THUMBNAILS)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool loadThumbnails(){return false;}")
ELSE()
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool loadThumbnails(){return true;}")
ENDIF()

IF(WITH_SEGMENTATION)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool useSegmentation(){return true;}")
ELSE()
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool useSegmentation(){return false;}")
ENDIF()

IF(WITHOUT_NEIGHBORS)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool useNeighbors(){return false;}")
ELSE()
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool useNeighbors(){return true;}")
ENDIF()

IF(SUPPORT_MULTIMEDIA)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
	#if defined (SUPPORT_MULTIMEDIA)
	virtual std::string mediaReaderPlugin(){return \"${MEDIA_READER}\";}
	virtual std::string activeMediaType(){return \"${ACTIVE_MEDIA}\";}
	#endif
")
ENDIF()

IF(WITH_OSC)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool useOSC(){return true;}\n")
ENDIF()
IF(CONNECT_OSC)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool connectOSC(){return true;}\n")
ENDIF()
IF(FULLSCREEN)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual bool fullscreen(){return true;}\n")
ENDIF()

IF(SETTING_INSTALLATION)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual ACSettingType settingType(){return AC_SETTING_INSTALLATION;}\n")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "};

#endif //__${CLASS_NAME}_H__
")

endmacro(CREATE_DEFAULT_CONFIG TARGET_NAME)
