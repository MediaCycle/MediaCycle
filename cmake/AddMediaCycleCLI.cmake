# Requires the following variables to be set:
#  MC_PLUGINS (list): example SET(MC_PLUGINS "segmentation;visualisation")
#  DESCRIPTION: example: SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
#  DEFAULT_CONFIGS (list): example SET(DEFAULT_CONFIGS "ACLoopCycleDefaultConfig;ACLoopJamDefaultConfig")
#  See CreateDefaultConfig.cmake for its required variables
# Optional:
#  MAIN_CLASS: that inherits from ACMultiMediaCycleOsgQt: example SET(MAIN_CLASS "ACAudioCycleLoopJam")
#  ICON_NAME: example SET(ICON_NAME "MultiMediaCycle")

macro(ADD_MC_CLI_EXECUTABLE APP_NAME)

IF(APPLE AND USE_DEBUG)
    GET_FILENAME_COMPONENT(TARGET_FOLDER ${CMAKE_CURRENT_BINARY_DIR} NAME)
    SET(TARGET_NAME ${TARGET_FOLDER})
    #MESSAGE("${TARGET_NAME}")
ELSEIF(UNIX AND NOT APPLE)
    string(TOLOWER ${TARGET_NAME} TARGET_NAME)
ELSE()
    SET(TARGET_NAME ${APP_NAME})
ENDIF()

IF(NOT MC_PLUGINS)
	MESSAGE(FATAL_ERROR "Warning, no plugin set, set the MC_PLUGINS list")
ENDIF()
IF(NOT DEFAULT_CONFIGS)
	FILE(GLOB DEFAULT_CONFIGS ${CMAKE_CURRENT_BINARY_DIR}/AC*DefaultConfig*.h)
ENDIF()
IF(NOT DEFAULT_CONFIGS)
	MESSAGE(FATAL_ERROR "Warning, no default config available, set the DEFAULT_CONFIGS list")
ENDIF()
#IF(NOT MAIN_CLASS)
#	SET(MAIN_CLASS "ACMultiMediaCycleOsgQt")
#ENDIF()

# Generate main.cpp
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
/*
 *  ${TARGET_NAME} main.cpp
 *  MediaCycle
 *
 *  Don't modify this file, rather its generator:
 *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
 */

#include <MediaCycle.h>
#include \"ACAbstractProcessCLI.h\"
#include \"ACAbstractDefaultConfig.h\"
")
IF(MAIN_CLASS)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "#include \"${MAIN_CLASS}.h\"\n")
ENDIF()

foreach(DEFAULT_CONFIG ${DEFAULT_CONFIGS})
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_HEADER ${DEFAULT_CONFIG} NAME)
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_PATH ${DEFAULT_CONFIG} PATH)
	INCLUDE_DIRECTORIES(${DEFAULT_CONFIG_PATH})
	#MESSAGE("Adding default config class: ${DEFAULT_CONFIG_HEADER} from path: ${DEFAULT_CONFIG_PATH}")
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "#include \"${DEFAULT_CONFIG_HEADER}\"\n")
endforeach(DEFAULT_CONFIG)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
int main(int argc, char *argv[])
{
    try {
        MediaCycle* media_cycle = new MediaCycle(MEDIA_TYPE_NONE);
")
SET(ACTION "load")
list(LENGTH DEFAULT_CONFIGS DEFAULT_CONFIGS_NUMBER)
IF(${DEFAULT_CONFIGS_NUMBER} GREATER 1)
	SET(ACTION "add")
ENDIF()
foreach(DEFAULT_CONFIG ${DEFAULT_CONFIGS})
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_CLASS ${DEFAULT_CONFIG} NAME_WE)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "\t\tmedia_cycle->${ACTION}DefaultConfig(new ${DEFAULT_CONFIG_CLASS}());\n")
endforeach(DEFAULT_CONFIG)

IF(MAIN_CLASS)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "\t\t${MAIN_CLASS}* main_class = new ${MAIN_CLASS}(media_cycle);
        if(main_class){
            main_class->run(argc,argv);
            delete main_class;
        }
")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
        if(media_cycle)
            delete media_cycle;
    }
    catch (const exception& e) {
        cout << \"** caught exception in main : \" << e.what() << endl;
    }
    catch (...){
        cout << \"** caught undetermined exception in main\" << endl;
    }
    return 1;
}
")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB ${TARGET_NAME}_SRCS ${CMAKE_CURRENT_BINARY_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB ${TARGET_NAME}_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_BINARY_DIR}/*.h)

IF(NOT ICON_NAME)
	SET(ICON_NAME "MultiMediaCycle")
ENDIF()

IF(WIN32 OR MINGW)
	EXECUTE_PROCESS(COMMAND echo "IDI_ICON1	ICON DISCARDABLE \"${CMAKE_SOURCE_DIR}/data/icons/${ICON_NAME}.ico\"" 
		OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc)
	EXECUTE_PROCESS(COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc 
		-o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.o)
	SET(OS_SPECIFIC "WIN32" ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.o)
ENDIF()
IF(APPLE)
	SET(APP_TYPE MACOSX_BUNDLE)
	SET(MACOSX_BUNDLE_BUNDLE_NAME ${TARGET_NAME})
	# set how it shows up in the Info.plist file
	SET(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}.icns) 
	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/data/icons/${ICON_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	# include the icns file in the target
	SET(${TARGET_NAME}_SRCS ${${TARGET_NAME}_SRCS} ${CMAKE_SOURCE_DIR}/data/icons/${ICON_NAME}.icns)
	SET(OS_SPECIFIC ${APP_TYPE} ${MACOSX_BUNDLE_INFO_PLIST})
ENDIF()

ADD_EXECUTABLE(${TARGET_NAME} ${OS_SPECIFIC} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})	

ADD_DEPENDENCIES(${TARGET_NAME} mediacycle)

TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)

IF(USE_OSC)
        TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-osc)
ENDIF()

foreach(PLUGIN ${MC_PLUGINS})
	ADD_DEPENDENCIES(${TARGET_NAME} ${PLUGIN_PREFIX}${PLUGIN})
endforeach(PLUGIN)


IF ( APPLE )
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
ENDIF ( APPLE )

endmacro(ADD_MC_CLI_EXECUTABLE APP_NAME)
