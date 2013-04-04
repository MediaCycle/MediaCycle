# Requires the following variables to be set:
#  MEDIA_TYPE, example: SET(MEDIA_TYPE "MEDIA_TYPE_AUDIO")
#  DESCRIPTION: example: SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
#  MC_PLUGINS (list): example SET(MC_PLUGINS "segmentation;visualisation")
#
# Required (osg)
#  OSG_PLUGINS (list): example SET(OSG_PLUGINS "osgdb_ffmpeg;osgdb_svg")
#
# Optional
#  BROWSER_MODE: example: SET(BROWSER_MODE "AC_MODE_CLUSTERS"), otherwise defaults to clusters mode
#  CLUSTERS_METHOD: example: SET(CLUSTERS_METHOD "MediaCycle KMeans")
#  CLUSTERS_POSITIONS: example: SET(CLUSTERS_POSITIONS "MediaCycle Propeller")
#  NEIGHBORS_METHOD
#  NEIGHBORS_POSITIONS
#  PREPROCESS
#  SEGMENTATION
#  WITH_SEGMENTATION: example: SET(WITH_SEGMENTATION ON)
#  WITHOUT_NEIGHBORS: example: SET(WITHOUT_NEIGHBORS ON)
#  MEDIA_READER: example: SET(MEDIA_READER "Archipel Reader")
#  ACTIVE_MEDIA: example: SET(ACTIVE_MEDIA "text")
#
# Optional (osg-qt)
#  WITH_OSC: example SET(WITH_OSC ON)
#  CONNECT_OSC: example SET(CONNECT_OSC ON)
#  FULLSCREEN: example SET(FULLSCREEN ON)
#  SETTING_INSTALLATION: example SET(SETTING_INSTALLATION ON)

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
IF(NOT MC_PLUGINS)
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
		SET(BASE_CLASS "ACOsgAbstractDefaultConfigQt")
		SET(CLASS_NAME "ACOsg${ROOT_NAME}DefaultConfigQt")
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

#include \"${BASE_CLASS}.h\"

class ${CLASS_NAME}: public ${BASE_CLASS} {

public:
    ${CLASS_NAME}() : ${BASE_CLASS}(){}
    virtual ~${CLASS_NAME}(){}
    virtual std::string name(){return \"${TARGET_NAME}\";}
    virtual std::string description(){return \"${DESCRIPTION}\";}
    virtual ACMediaType mediaType(){return ${MEDIA_TYPE};}
    virtual ACBrowserMode browserMode(){return ${BROWSER_MODE};}
    virtual std::vector<std::string> pluginLibraries(){
        std::vector<std::string> plugins;
")

foreach(PLUGIN ${MC_PLUGINS})
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\tplugins.push_back(\"${PLUGIN}\");\n")
endforeach(PLUGIN)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\t\treturn plugins;
    }
")

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
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "
	virtual std::string clustersMethodPlugin(){return \"${CLUSTERS_METHOD}\";}
	virtual std::string clustersPositionsPlugin(){return \"${CLUSTERS_POSITIONS}\";}
	virtual std::string neighborsMethodPlugin(){return \"${NEIGHBORS_METHOD}\";}
	virtual std::string neighborsPositionsPlugin(){return \"${NEIGHBORS_POSITIONS}\";}
	virtual std::string preProcessPlugin(){return \"${PREPROCESS}\";}
	virtual std::string segmentationPlugin(){return \"${SEGMENTATION}\";}
")

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
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "\tvirtual ACSettingType settingType(){return AC_SETTING_DESKTOP;}\n")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${CLASS_NAME}.h "};

#endif //__${CLASS_NAME}_H__
")

endmacro(CREATE_DEFAULT_CONFIG TARGET_NAME)
