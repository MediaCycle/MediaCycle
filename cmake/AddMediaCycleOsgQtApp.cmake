# Requires the following variables to be set:
#  MC_PLUGINS (list): example SET(MC_PLUGINS "segmentation;visualisation")
#  DESCRIPTION: example: SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
#  DEFAULT_CONFIGS (list): example SET(DEFAULT_CONFIGS "ACLoopCycleDefaultConfig;ACLoopJamDefaultConfig")
#  See CreateDefaultConfig.cmake for its required variables
#
# Required (osg)
#  OSG_PLUGINS (list): example SET(OSG_PLUGINS "osgdb_ffmpeg;osgdb_svg")
#
# Optional:
#  MAIN_CLASS: that inherits from ACMultiMediaCycleOsgQt: example SET(MAIN_CLASS "ACAudioCycleLoopJam")
#  ICON_NAME: example SET(ICON_NAME "MultiMediaCycle")

include(${CMAKE_SOURCE_DIR}/cmake/AddQtApp.cmake)
macro(ADD_MC_OSG_QT_EXECUTABLE APP_NAME)

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
IF(NOT MAIN_CLASS)
	SET(MAIN_CLASS "ACMultiMediaCycleOsgQt")
ENDIF()

# Generate main.cpp
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
/*
 *  ${TARGET_NAME} main.cpp
 *  MediaCycle
 *
 *  Don't modify this file, rather its generator:
 *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
 */

#include <QApplication>
#include <QtGui>
#ifndef USE_DEBUG
#include <BreakpadHttpSender.h>
#include <BreakPadHandler.h>
#endif
#include \"${MAIN_CLASS}.h\"
")

foreach(DEFAULT_CONFIG ${DEFAULT_CONFIGS})
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_HEADER ${DEFAULT_CONFIG} NAME)
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_PATH ${DEFAULT_CONFIG} PATH)
	INCLUDE_DIRECTORIES(${DEFAULT_CONFIG_PATH})
	#MESSAGE("Adding default config class: ${DEFAULT_CONFIG_HEADER} from path: ${DEFAULT_CONFIG_PATH}")
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "#include \"${DEFAULT_CONFIG_HEADER}\"\n")
endforeach(DEFAULT_CONFIG)

IF(CMAKE_TOOLCHAIN_FILE AND MINGW AND OSG_PLUGINS)
        foreach(OSG_PLUGIN ${OSG_PLUGINS})
            string(REGEX MATCH ".*svg.*" WITH_OSGDB_SVG "${OSG_PLUGIN}")
            string(REGEX MATCH ".*ffmpeg.*" WITH_OSGDB_FFMPEG "${OSG_PLUGIN}")
            IF(WITH_OSGDB_SVG AND RSVG_FOUND)
		file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "USE_OSGPLUGIN(SVG)\n")
            ELSEIF(WITH_OSGDB_FFMPEG AND FFMPEG_FOUND)
		file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "USE_OSGPLUGIN(ffmpeg)\n")
            ELSE()
                string(REPLACE "osgdb_" ""  OSG_PLUGIN "${OSG_PLUGIN}")
                file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "USE_OSGPLUGIN(${OSG_PLUGIN})\n")
            ENDIF()
        endforeach(OSG_PLUGIN)
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName(\"UMONS/numediart\");
	app.setOrganizationDomain(\"mediacycle.org\");
	app.setApplicationName(\"${TARGET_NAME}\");

	// Make Apple *.app bundles not load installed Qt Frameworks but load Qt Plugins
#ifdef __APPLE__
	QApplication::setLibraryPaths(QStringList(QApplication::applicationDirPath() + \"/../PlugIns\"));
#endif
	#ifndef USE_DEBUG
        BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String(\"crashes\"));
	#endif

	${MAIN_CLASS} window;

	#ifdef USE_DEBUG
	try {
	#endif
		window.setWindowTitle(\"${TARGET_NAME}\");
		window.show();
")
SET(ACTION "load")
list(LENGTH DEFAULT_CONFIGS DEFAULT_CONFIGS_NUMBER)
IF(${DEFAULT_CONFIGS_NUMBER} GREATER 1)
	SET(ACTION "add")
ENDIF()
foreach(DEFAULT_CONFIG ${DEFAULT_CONFIGS})
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_CLASS ${DEFAULT_CONFIG} NAME_WE)
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "\t\twindow.${ACTION}DefaultConfig(new ${DEFAULT_CONFIG_CLASS}());\n")
endforeach(DEFAULT_CONFIG)

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
	#ifdef USE_DEBUG
	}
	catch (const exception& e) {
		cout << \"** caught exception in main : \" << e.what() << endl;
	}
	catch (...){
		cout << \"** caught undetermined exception in main\" << endl;
	}
	#endif
	return app.exec();
}
")

# For AddQtApp.cmake
SET(WITH_MC ON)
SET(WITH_OSG ON)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/gui-osg-qt ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_QT_EXECUTABLE(${TARGET_NAME})	

ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-osg-qt)

foreach(OSG_PLUGIN ${OSG_PLUGINS})
	IF(IS_DIRECTORY("${CMAKE_SOURCE_DIR}/3rdparty/${OSG_PLUGIN}"))
		ADD_DEPENDENCIES(${TARGET_NAME} ${OSG_PLUGIN})
	ENDIF()
endforeach(OSG_PLUGIN)
IF(CMAKE_TOOLCHAIN_FILE AND MINGW AND OSG_PLUGINS)
    foreach(OSG_PLUGIN ${OSG_PLUGINS})
        string(REGEX MATCH ".*svg.*" WITH_OSGDB_SVG "${OSG_PLUGIN}")
        string(REGEX MATCH ".*ffmpeg.*" WITH_OSGDB_FFMPEG "${OSG_PLUGIN}")
        IF(WITH_OSGDB_SVG AND WITH_OSGDB_SVG AND RSVG_FOUND)
		TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_svg)
        ELSEIF(WITH_OSGDB_FFMPEG AND WITH_OSGDB_FFMPEG AND FFMPEG_FOUND)
		TARGET_LINK_LIBRARIES(${TARGET_NAME} osgdb_ffmpeg)
        ELSE()
                TARGET_LINK_LIBRARIES(${TARGET_NAME} -L${OSG_LIBRARY_DIR}/osgPlugins-${OPENSCENEGRAPH_VERSION} -l${OSG_PLUGIN})
        ENDIF()
    endforeach(OSG_PLUGIN)
ENDIF()

TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle mediacycle-osg mediacycle-osg-qt)

IF(USE_OSC)
        TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-osc)
ENDIF()

foreach(PLUGIN ${MC_PLUGINS})
	ADD_DEPENDENCIES(${TARGET_NAME} ${PLUGIN_PREFIX}${PLUGIN})
endforeach(PLUGIN)

IF ( APPLE )
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	IF ( USE_APPLE_MULTITOUCH )
		#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
		TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )
ENDIF ( APPLE )

endmacro(ADD_MC_OSG_QT_EXECUTABLE APP_NAME)
