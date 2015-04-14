#=============================================================================
# Author: Christian Frisson
# Copyright (c) 2015 – UMONS - Numediart
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

# Requires the following variables to be set:
#  MC_PLUGINS (list): example SET(MC_PLUGINS "segmentation;visualisation")
#  DESCRIPTION: example: SET(DESCRIPTION "MediaCycle allows to organize media files by similarity")
#  DEFAULT_CONFIGS (list): example SET(DEFAULT_CONFIGS "ACLoopCycleDefaultConfig;ACLoopJamDefaultConfig")
#  See CreateDefaultConfig.cmake for its required variables
#
# Required (Webkit view)
#  LOCAL_LIBRARY_FILENAME: example SET(LOCAL_LIBRARY_FILENAME "library.json")
#  LOCAL_LIBRARY_WRITER_PLUGIN_NAME: example SET(LOCAL_LIBRARY_WRITER_PLUGIN_NAME "JSON export")
#  WEBPAGE_RELATIVE_SOURCEPATH: example SET(WEBPAGE_RELATIVE_SOURCEPATH "3rdparty/audio-gui-html5/index.html")
#
# Optional:
#  MC_PLUGINS_STATIC: example SET(MC_PLUGINS_STATIC ON)
#  MAIN_CLASS: that inherits from ACMediaCycleWebkitQt: example SET(MAIN_CLASS "ACMediaCycleWebkitQt")
#  ICON_NAME: example SET(ICON_NAME "MultiMediaCycle")
#  HIDE_INFORMATION: example SET(HIDE_INFORMATION ON)

include(${CMAKE_SOURCE_DIR}/cmake/AddQtApp.cmake)
macro(ADD_MC_WEBKIT_QT_EXECUTABLE APP_NAME)

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

# Required (Webkit view)
IF(NOT LOCAL_LIBRARY_FILENAME)
        MESSAGE(FATAL_ERROR "Warning, LOCAL_LIBRARY_FILENAME must be set, example SET(LOCAL_LIBRARY_FILENAME \"library.json\")")
ENDIF()
IF(NOT LOCAL_LIBRARY_WRITER_PLUGIN_NAME)
        MESSAGE(FATAL_ERROR "Warning, LOCAL_LIBRARY_WRITER_PLUGIN_NAME must be set, example SET(LOCAL_LIBRARY_WRITER_PLUGIN_NAME \"JSON export\")")
ENDIF()
IF(NOT WEBPAGE_RELATIVE_SOURCEPATH)
        MESSAGE(FATAL_ERROR "Warning, WEBPAGE_RELATIVE_SOURCEPATH must be set, example SET(WEBPAGE_RELATIVE_SOURCEPATH \"3rdparty/audio-gui-html5/index.html\")")
ENDIF()

IF(NOT MAIN_CLASS)
        SET(MAIN_CLASS "ACMediaCycleWebkitQt")
ENDIF()

IF(MC_PLUGINS_STATIC)
    ADD_DEFINITIONS(-DMC_PLUGINS_STATIC)
ENDIF()

# Generate the class that inherits from ACMultiMediaCycleQt and embeds an ACWebkitViewQt view
SET(MAIN_WINDOW_CLASS_NAME "AC${APP_NAME}WebkitQt")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${MAIN_WINDOW_CLASS_NAME}.h "
/**
 * @brief Main ${APP_NAME} window wrapping a webkit view of a ${APP_NAME} browser
 * @author Christian Frisson
 * @date 05/03/2015
 * @copyright (c) 2015 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#ifndef HEADER_${MAIN_WINDOW_CLASS_NAME}
#define HEADER_${MAIN_WINDOW_CLASS_NAME}

#include \"ACMultiMediaCycleQt.h\"
#include \"ACWebkitViewQt.h\"

class ${MAIN_WINDOW_CLASS_NAME} : public ACMultiMediaCycleQt {
    //Q_OBJECT

public:
    ${MAIN_WINDOW_CLASS_NAME}(QWidget *parent = 0) : ACMultiMediaCycleQt(new ACWebkitViewQt())
    {
        ACMediaFactory::getInstance().useRendering(false);
    }
    ~${MAIN_WINDOW_CLASS_NAME}(){}

    // From ACMultiMediaCycleQt
    /// This will be run right after the default config has been loaded
    virtual void postLoadDefaultConfig(){
        ACWebkitViewQt* webkitView = dynamic_cast<ACWebkitViewQt*>(this->view);
        if(webkitView){
            webkitView->setMediaLibraryBaseFileName(\"${LOCAL_LIBRARY_FILENAME}\");
            webkitView->setMediaLibraryWriterPluginName(\"${LOCAL_LIBRARY_WRITER_PLUGIN_NAME}\");
            webkitView->setWebpageRelativeSourcePath(\"${WEBPAGE_RELATIVE_SOURCEPATH}\");
        }
    }
protected:
    ACWebkitViewQt* webkitView;
};
#endif // HEADER_${MAIN_WINDOW_CLASS_NAME}
")

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
")

IF(USE_QT4 AND NOT USE_QT5)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
#ifndef USE_DEBUG
#include <BreakpadHttpSender.h>
#include <BreakPadHandler.h>
#endif
")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
#include \"${MAIN_WINDOW_CLASS_NAME}.h\"
")

foreach(DEFAULT_CONFIG ${DEFAULT_CONFIGS})
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_HEADER ${DEFAULT_CONFIG} NAME)
	GET_FILENAME_COMPONENT(DEFAULT_CONFIG_PATH ${DEFAULT_CONFIG} PATH)
	INCLUDE_DIRECTORIES(${DEFAULT_CONFIG_PATH})
	#MESSAGE("Adding default config class: ${DEFAULT_CONFIG_HEADER} from path: ${DEFAULT_CONFIG_PATH}")
	file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "#include \"${DEFAULT_CONFIG_HEADER}\"\n")
endforeach(DEFAULT_CONFIG)

SET(MC_PLUGIN_LIBRARIES)
SET(DIRECTORIES_TO_INCLUDE)
FOREACH(PLUGIN_NAME ${MC_PLUGINS})

    IF(CMAKE_CAN_APPEND_PROPERTIES)

        GET_TARGET_PROPERTY(PLUGIN_MC_3RDPARTIES "mc_${PLUGIN_NAME}" MC_3RDPARTIES)
        GET_TARGET_PROPERTY(PLUGIN_MC_LIBS "mc_${PLUGIN_NAME}" MC_LIBS)
        GET_TARGET_PROPERTY(PLUGIN_MC_MEDIA_LIBS "mc_${PLUGIN_NAME}" MC_MEDIA_LIBS)
        STRING(REGEX REPLACE "PLUGIN_MC_3RDPARTIES-NOTFOUND" "" PLUGIN_MC_3RDPARTIES ${PLUGIN_MC_3RDPARTIES})
        STRING(REGEX REPLACE "PLUGIN_MC_LIBS-NOTFOUND" "" PLUGIN_MC_LIBS ${PLUGIN_MC_LIBS})
        STRING(REGEX REPLACE "PLUGIN_MC_MEDIA_LIBS-NOTFOUND" "" PLUGIN_MC_MEDIA_LIBS ${PLUGIN_MC_MEDIA_LIBS})
        FOREACH(PLUGIN_MC_3RDPARTY ${PLUGIN_MC_3RDPARTIES})
            #MESSAGE("PLUGIN_MC_3RDPARTY ${PLUGIN_MC_3RDPARTY}")
            GET_TARGET_PROPERTY(PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES "${PLUGIN_MC_3RDPARTY}" INCLUDE_DIRECTORIES)
            #STRING(MATCHES "PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES-NOTFOUND" PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES_NOT_FOUND ${PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES})
		list(FIND PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES "PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES-NOTFOUND" PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES_NOT_FOUND)
		IF(PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES_NOT_FOUND MATCHES "-1")
			FOREACH(PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORY ${PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORIES})
                		LIST(APPEND DIRECTORIES_TO_INCLUDE "${PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORY}")
            		ENDFOREACH(PLUGIN_MC_3RDPARTY_INCLUDE_DIRECTORY)
		ENDIF()
        ENDFOREACH(PLUGIN_MC_3RDPARTY)
        #MESSAGE("Plugin mc_${PLUGIN_NAME} 3rd parties: '${PLUGIN_MC_3RDPARTIES}' libs: '${PLUGIN_MC_LIBS}' media libs: '${PLUGIN_MC_MEDIA_LIBS}'" )
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
        INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/plugins/${PLUGIN_NAME}")
	INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/plugins/${PLUGIN_NAME}")
        LIST(APPEND MC_PLUGIN_LIBRARIES ${PLUGIN_LIBRARY_NAME})
    #ENDIF()
ENDFOREACH(PLUGIN_NAME)
IF(DIRECTORIES_TO_INCLUDE)
    LIST(REMOVE_DUPLICATES DIRECTORIES_TO_INCLUDE)
    #MESSAGE("DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE}")
    INCLUDE_DIRECTORIES(${DIRECTORIES_TO_INCLUDE})
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "

class ACApplicationWebkitQt : public QApplication {
public:
  ACApplicationWebkitQt(int& argc, char ** argv) :
    QApplication(argc, argv) { }
  virtual ~ACApplicationWebkitQt() { }

  // reimplemented from QApplication so we can throw exceptions in slots
  virtual bool notify(QObject * receiver, QEvent * event) {
    try {
      return QApplication::notify(receiver, event);
    } catch(std::exception& e) {
      qCritical() << \"Exception thrown:\" << e.what();
      const QString qs = QString::fromStdString(e.what());
      //if(this->setting != AC_SETTING_INSTALLATION)
      int warn_button = QMessageBox::warning(0,\"Error\", qs,QMessageBox::Ok,QMessageBox::NoButton);
    }
    return false;
  }
};

int main(int argc, char *argv[])
{
        ACApplicationWebkitQt app(argc, argv);
	app.setOrganizationName(\"UMONS/numediart\");
	app.setOrganizationDomain(\"mediacycle.org\");
	app.setApplicationName(\"${TARGET_NAME}\");

	// Make Apple *.app bundles not load installed Qt Frameworks but load Qt Plugins
#if defined (__APPLE__) and !defined(USE_DEBUG)
	QApplication::setLibraryPaths(QStringList(QApplication::applicationDirPath() + \"/../PlugIns\"));
#endif
")

IF(USE_QT4 AND NOT USE_QT5)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
        #ifndef USE_DEBUG
        BreakpadQt::GlobalHandler::instance()->setDumpPath(QLatin1String(\"crashes\"));
        #endif
")
ENDIF()

file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "
        ${MAIN_WINDOW_CLASS_NAME} window;

	#ifdef USE_DEBUG
	try {
	#endif
		window.setWindowTitle(\"${TARGET_NAME}\");
                //window.show();
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
        window.commandLine(argc,argv);
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/gui-qt ${CMAKE_BINARY_DIR}/libs/gui-qt )
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/gui-webkit-qt ${CMAKE_BINARY_DIR}/libs/gui-webkit-qt )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_QT_EXECUTABLE(${TARGET_NAME})

ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-webkit-qt)

# GCC flag to strip unneeded symbols
IF(MINGW AND CMAKE_TOOLCHAIN_FILE AND MC_PLUGINS_STATIC)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
ENDIF()

LIST(APPEND KNOWN_CLASHING_LIBS "curl+glib") # multiple definition of `g_utf8_skip', idn linked by curl
LIST(APPEND KNOWN_CLASHING_LIBS "portaudio+dxguid") # portaudio contains dxguid linked by others (for instance ffmpeg)
#MESSAGE("KNOWN_CLASHING_LIBS ${KNOWN_CLASHING_LIBS}")

foreach(KNOWN_CLASHING_LIB ${KNOWN_CLASHING_LIBS})
    string(REPLACE "+" ";" CLASHING_LIBS ${KNOWN_CLASHING_LIB})
    #MESSAGE("CLASHING_LIBS: ")
    foreach(CLASHING_LIB ${CLASHING_LIBS})
        #MESSAGE("${CLASHING_LIB}")
        LIST(APPEND ALL_KNOWN_CLASHING_LIBS ${CLASHING_LIB})
    endforeach(CLASHING_LIB)
endforeach(KNOWN_CLASHING_LIB)

#MESSAGE("ALL_KNOWN_CLASHING_LIBS ${ALL_KNOWN_CLASHING_LIBS}")

foreach(PLUGIN_NAME ${MC_PLUGINS})
    IF(MC_PLUGINS_STATIC)
        SET(LIB_DEPENDS ${${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin_LIB_DEPENDS})
        IF(LIB_DEPENDS)
            LIST(REMOVE_ITEM LIB_DEPENDS "mediacycle")
            LIST(REMOVE_ITEM LIB_DEPENDS "general")
            #MESSAGE("LIB_DEPENDS ${LIB_DEPENDS}")
            foreach(LIB_DEPEND ${LIB_DEPENDS})

                SET(MC_LIB_DEPENDS ${${LIB_DEPEND}_LIB_DEPENDS})
                IF(MC_LIB_DEPENDS)
                    #MESSAGE("MC_LIB_DEPENDS ${MC_LIB_DEPENDS}")
                    foreach(KNOWN_CLASHING_LIB ${ALL_KNOWN_CLASHING_LIBS})
                        STRING(REGEX MATCH "${KNOWN_CLASHING_LIB}" CONTAINS_CLASH "${MC_LIB_DEPENDS}")
                        IF(CONTAINS_CLASH)
                            LIST(APPEND ACTUAL_CLASHING_LIBS "${KNOWN_CLASHING_LIB} @ ${LIB_DEPEND} from ${PLUGIN_NAME}")
                            #MESSAGE("\nCLASH ${KNOWN_CLASHING_LIB} @ ${LIB_DEPEND} from ${PLUGIN_NAME}\n")
                        ENDIF()
                    endforeach(KNOWN_CLASHING_LIB)
                ENDIF()
                STRING(REGEX MATCH "mediacycle-[a-zA-Z0-9-]+" IS_MC_LIB "${LIB_DEPEND}")
                IF(IS_MC_LIB)
                    STRING(REGEX REPLACE "mediacycle-" "" MC_LIB "${LIB_DEPEND}")
                    #MESSAGE("LIB_DEPEND: ${LIB_DEPEND} -> ${MC_LIB}")
                    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/${MC_LIB})
                ENDIF()
            endforeach(LIB_DEPEND ${LIB_DEPENDS})
        ENDIF()
        ADD_DEPENDENCIES(${TARGET_NAME} ${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin)
        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin)
    ELSE()
        ADD_DEPENDENCIES(${TARGET_NAME} ${PLUGIN_PREFIX}${PLUGIN_NAME})
    ENDIF()
endforeach(PLUGIN_NAME)

IF(MC_PLUGINS_STATIC)
    SET(TOTAL_CLASHES)
    foreach(KNOWN_CLASHING_LIB ${KNOWN_CLASHING_LIBS})
        string(REPLACE "+" ";" CLASHING_LIBS ${KNOWN_CLASHING_LIB})
        #MESSAGE("CLASHING_LIBS: ")
        SET(CLASH_REASON)
        foreach(CLASHING_LIB ${CLASHING_LIBS})
            #MESSAGE("${CLASHING_LIB}")
            foreach(ACTUAL_CLASHING_LIB ${ACTUAL_CLASHING_LIBS})
                STRING(REGEX MATCH "${CLASHING_LIB}" IS_CLASHING "${ACTUAL_CLASHING_LIB}")
                IF(IS_CLASHING)
                    LIST(APPEND CLASH_REASON "${ACTUAL_CLASHING_LIB}")
                ENDIF()
            endforeach(ACTUAL_CLASHING_LIB)
        endforeach(CLASHING_LIB)
        LIST(LENGTH CLASH_REASON CLASH_COUNT)
        LIST(LENGTH CLASHING_LIBS CLASHING_LIBS_COUNT)
        #MESSAGE("CLASH ${CLASH_REASON} ${CLASH_COUNT}/${CLASHING_LIBS_COUNT}")
        IF(CLASH_COUNT EQUAL CLASHING_LIBS_COUNT)
            string(REPLACE ";" " with " CLASH_REASON "${CLASH_REASON}")
            LIST(APPEND TOTAL_CLASHES ${CLASH_REASON})
        ENDIF()
    endforeach(KNOWN_CLASHING_LIB)
    LIST(LENGTH TOTAL_CLASHES TOTAL_CLASHES_COUNT)
    IF(TOTAL_CLASHES_COUNT GREATER 0)
        string(REPLACE ";" ", and " TOTAL_CLASHES "${TOTAL_CLASHES}")
        #MESSAGE(FATAL_ERROR "Target ${TARGET_NAME} won't link due to clashes between ${TOTAL_CLASHES}. Disable the required options or the target and re-run CMake.")
    ENDIF()
ENDIF()

TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-webkit-qt)

IF ( APPLE )
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreVideo -framework AppKit -framework Foundation -framework AudioToolbox")
	IF ( USE_APPLE_MULTITOUCH )
		#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /System/Library/PrivateFrameworks/MultitouchSupport.framework/MultitouchSupport")
		TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-io-applemultitouch)
	ENDIF ( USE_APPLE_MULTITOUCH )
ENDIF ( APPLE )

IF(APPLE)# AND NOT USE_DEBUG)
    IF(XCODE)
        SET(EXTRA_PATH "/Debug")
    ENDIF()
    GET_FILENAME_COMPONENT(WEBPAGE_RELATIVE_SOURCEDIR ${WEBPAGE_RELATIVE_SOURCEPATH} PATH)
    ADD_CUSTOM_COMMAND( TARGET ${TARGET_NAME} POST_BUILD
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}${EXTRA_PATH}/${TARGET_NAME}.app/Contents/Resources/
        COMMAND cp -R "${CMAKE_SOURCE_DIR}/${WEBPAGE_RELATIVE_SOURCEDIR}/*" ${CMAKE_CURRENT_BINARY_DIR}${EXTRA_PATH}/${TARGET_NAME}.app/Contents/Resources/
    )
ENDIF()

endmacro(ADD_MC_WEBKIT_QT_EXECUTABLE APP_NAME)
