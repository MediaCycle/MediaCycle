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

# Optional variables to be set:
#  MC_3RDPARTIES (list): example SET(MC_3RDPARTIES "cvBlobsLib;cvgabor")
#  MC_LIBS (list): example SET(MC_LIBS "io-osc"), without their prefix mediacycle-
#  MC_MEDIA_LIBS (list): example SET(MC_MEDIA_LIBS "image-core;image-features"), without their prefix mediacycle-
#  3RDPARTY_NAMES (list): example list(APPEND 3RDPARTY_NAMES "cvBlobsLib")
#  3RDPARTY_LICENSES (list): example list(APPEND 3RDPARTY_LICENSES "free")
#  3RDPARTY_URLS (list): example list(APPEND 3RDPARTY_URLS "http://opencv.willowgarage.com/wiki/cvBlobsLib")
#  NO_SHARED: example SET(NO_SHARED ON)
#
# Required (osg)
#  OSG_PLUGINS (list): example SET(OSG_PLUGINS "osgdb_ffmpeg;osgdb_svg")
#

macro(ADD_MC_PLUGIN_LIBRARY PLUGIN_NAME)

    SET(TARGET_NAME "${PLUGIN_PREFIX}${PLUGIN_NAME}")
    SET(LIBRARY_NAME "${LIBRARY_PREFIX}mediacycle-${PLUGIN_NAME}-plugin")
    SET(MEDIA_TYPE "${PLUGIN_NAME}")


    FILE(GLOB ${TARGET_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)
    FILE(GLOB ${TARGET_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hh ${CMAKE_CURRENT_BINARY_DIR}/*.h)
    

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

    LIST(REMOVE_ITEM ${TARGET_NAME}_SRC "${CMAKE_CURRENT_BINARY_DIR}/factory.cpp")
    LIST(REMOVE_ITEM ${TARGET_NAME}_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h")

    # Get the list of MediaCycle plugin classes and names (mName):
    # Assumes one plugin per header and header/class file named like class
    FILE(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    foreach(HEADER ${HEADERS})
        GET_FILENAME_COMPONENT(CLASS ${HEADER} NAME_WE)
        file(READ "${HEADER}" HEADER_CONTENTS)
        string(REGEX MATCH "public[ \t]+AC[a-zA-Z0-9]+lugin" IS_MC_PLUGIN "${HEADER_CONTENTS}")
        IF(IS_MC_PLUGIN)
            file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${CLASS}.cpp" CLASS_CONTENTS)
            string(REGEX MATCH "mName[ \t]+=[ \t]+\"[^\n\r]+\"[ \t;]" NAME_LINE "${CLASS_CONTENTS}")
            IF(NAME_LINE)
                string(REGEX REPLACE "mName[ \t]+=[ \t]+\"([^\n\r]+)\"[ \t;]" "\\1" PLUGIN_STRING "${NAME_LINE}")
                list(APPEND PLUGIN_CLASSES "${CLASS}")
                list(APPEND PLUGIN_STRINGS "${PLUGIN_STRING}")
                list(APPEND CREATE_LINES "\tif (namePlugin == \"${PLUGIN_STRING}\") {return new ${CLASS}()\;}")
                #MESSAGE("Plugin class '${CLASS}' and name: '${PLUGIN_STRING}'")
            ENDIF()
        ENDIF()
    endforeach(HEADER)

    IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp)
        execute_process(COMMAND "rm" "${CMAKE_CURRENT_BINARY_DIR}/factory.cpp")
    ENDIF()

    # Generate factory.cpp
    IF(NOT NO_SHARED)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp 
            "/*
            *  ${TARGET_NAME} factory.cpp
            *  MediaCycle
            *
            *  Don't modify this file, rather its generator:
            *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
            */
        ")

        IF(CMAKE_TOOLCHAIN_FILE AND MINGW AND NOT NO_SHARED)
            string(REGEX MATCH ".*osg.*" WITH_OSG "${MC_MEDIA_LIBS}")
            IF(OSG_PLUGINS)
                file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "
                    #ifdef OSG_LIBRARY_STATIC
                    #include <osgDB/ReadFile>\n")

                foreach(OSG_PLUGIN ${OSG_PLUGINS})
                    string(REGEX MATCH ".*svg.*" WITH_OSGDB_SVG "${OSG_PLUGIN}")
                    string(REGEX MATCH ".*ffmpeg.*" WITH_OSGDB_FFMPEG "${OSG_PLUGIN}")
                    IF(WITH_OSGDB_SVG AND WITH_OSGDB_SVG AND RSVG_FOUND)
                        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "USE_OSGPLUGIN(SVG)\n")
                    ELSEIF(WITH_OSGDB_FFMPEG AND WITH_OSGDB_FFMPEG AND FFMPEG_FOUND)
                        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "USE_OSGPLUGIN(ffmpeg)\n")
                    ELSE()
                        string(REPLACE "osgdb_" ""  OSG_PLUGIN "${OSG_PLUGIN}")
                        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "USE_OSGPLUGIN(${OSG_PLUGIN})\n")
                    ENDIF()
                endforeach(OSG_PLUGIN)

                file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "#endif\n")
            ENDIF()
        ENDIF()


        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "
            #include <ACPlugin.h>
        ")

        foreach(PLUGIN_CLASS ${PLUGIN_CLASSES})
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "#include \"${PLUGIN_CLASS}.h\"\n")
        endforeach(PLUGIN_CLASS)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\nextern \"C\" ACPlugin* create(std::string namePlugin) {\n")
        foreach(CREATE_LINE ${CREATE_LINES})
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "${CREATE_LINE}\n")
        endforeach(CREATE_LINE)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\telse return 0;
            }\n
            extern \"C\" void destroy(ACPlugin* d) {
            delete d;
            }\n
            extern \"C\" std::vector<std::string> list() {
            std::vector<std::string> listPlugin;
        ")
        foreach(PLUGIN_STRING ${PLUGIN_STRINGS})
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\tlistPlugin.push_back(\"${PLUGIN_STRING}\");\n")
        endforeach(PLUGIN_STRING)
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\treturn listPlugin;
            }\n
            extern \"C\" std::vector<ACThirdPartyMetadata> dependencies() {
            std::vector<ACThirdPartyMetadata> thirdPartyMetadata;
        ")
        list(LENGTH 3RDPARTY_NAMES 3RDPARTY_NAMES_NUMBER)
        list(LENGTH 3RDPARTY_LICENSES 3RDPARTY_LICENSES_NUMBER)
        list(LENGTH 3RDPARTY_URLS 3RDPARTY_URLS_NUMBER)
        IF(NOT(${3RDPARTY_NAMES_NUMBER} EQUAL 0) AND NOT(${3RDPARTY_LICENSES_NUMBER} EQUAL 0) AND NOT(${3RDPARTY_URLS_NUMBER} EQUAL 0))
            IF(NOT(${3RDPARTY_NAMES_NUMBER} EQUAL ${3RDPARTY_LICENSES_NUMBER}) OR NOT(${3RDPARTY_NAMES_NUMBER} EQUAL ${3RDPARTY_URLS_NUMBER}))
                MESSAGE(FATAL_ERROR "The 3rdparty names/licenses/numbers lists for plugin ${TARGET_NAME} have different sizes, check ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
            ENDIF()
            math(EXPR NUMBER "${3RDPARTY_NAMES_NUMBER} - 1")
            foreach(3RDPARTY RANGE 0 ${NUMBER})
                list(GET 3RDPARTY_NAMES ${3RDPARTY} 3RDPARTY_NAME)
                list(GET 3RDPARTY_LICENSES ${3RDPARTY} 3RDPARTY_LICENSE)
                list(GET 3RDPARTY_URLS ${3RDPARTY} 3RDPARTY_URL)
                file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\tthirdPartyMetadata.push_back(ACThirdPartyMetadata(\"${3RDPARTY_NAME}\",\"${3RDPARTY_LICENSE}\",\"${3RDPARTY_URL}\"));\n")
            endforeach(3RDPARTY)
        ENDIF()
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp "\treturn thirdPartyMetadata;
            }
        ")
    ENDIF(NOT NO_SHARED)

    # Generate ${TARGET_NAME}.cpp
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp
        "/*
        *  ${PLUGIN_LIBRARY_NAME}.cpp
        *  MediaCycle
        *
        *  Don't modify this file, rather its generator:
        *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        */

    ")

    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "#include <${PLUGIN_LIBRARY_NAME}.h>\n")

    # This is a placeholder for all definitions/headers that can't be included in ${PLUGIN_LIBRARY_NAME}.h if they break things when compiling statically
    IF(CMAKE_TOOLCHAIN_FILE AND MINGW)
        string(REGEX MATCH ".*osg.*" WITH_OSG "${MC_MEDIA_LIBS}")
        IF(WITH_OSG AND NOT OSG_PLUGINS)
            MESSAGE("Warning: no required OSG format plugin?")
        ENDIF()
        IF(OSG_PLUGINS)
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "
                #ifdef OSG_LIBRARY_STATIC
            #include <osgDB/ReadFile>\n")
            foreach(OSG_PLUGIN ${OSG_PLUGINS})
                string(REGEX MATCH ".*svg.*" WITH_OSGDB_SVG "${OSG_PLUGIN}")
                string(REGEX MATCH ".*ffmpeg.*" WITH_OSGDB_FFMPEG "${OSG_PLUGIN}")
                IF(WITH_OSGDB_SVG AND WITH_OSGDB_SVG AND RSVG_FOUND)
                    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "USE_OSGPLUGIN(SVG)\n")
                ELSEIF(WITH_OSGDB_FFMPEG AND WITH_OSGDB_FFMPEG AND FFMPEG_FOUND)
                    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "USE_OSGPLUGIN(ffmpeg)\n")
                ELSE()
                    string(REPLACE "osgdb_" ""  OSG_PLUGIN "${OSG_PLUGIN}")
                    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "USE_OSGPLUGIN(${OSG_PLUGIN})\n")
                ENDIF()
            endforeach(OSG_PLUGIN)
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "#endif\n")
        ENDIF()
    ENDIF()

    foreach(PLUGIN_CLASS ${PLUGIN_CLASSES})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "#include \"${PLUGIN_CLASS}.h\"\n")
    endforeach(PLUGIN_CLASS)

    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "${PLUGIN_LIBRARY_NAME}::${PLUGIN_LIBRARY_NAME}() : ACStaticPluginsLibrary() {
    ")
    foreach(PLUGIN_CLASS ${PLUGIN_CLASSES})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "\tmPlugins.push_back(new ${PLUGIN_CLASS}());\n")
    endforeach(PLUGIN_CLASS)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp "}")

    # Generate ${TARGET_NAME}.h
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h
    "/*
    *  ${PLUGIN_LIBRARY_NAME}.h
    *  MediaCycle
    *
    *  Don't modify this file, rather its generator:
    *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    */

    #ifndef _${PLUGIN_LIBRARY_NAME}
    #define _${PLUGIN_LIBRARY_NAME}

    #include <ACPluginLibrary.h>
    ")

    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h "\nclass ${PLUGIN_LIBRARY_NAME} : public ACStaticPluginsLibrary{
    public:
    ${PLUGIN_LIBRARY_NAME}();")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h "
        virtual ~${PLUGIN_LIBRARY_NAME}(){}
        virtual std::string getName(){return \"${TARGET_NAME}\";}
        virtual std::vector<ACThirdPartyMetadata> getThirdPartyMetadata() {
        std::vector<ACThirdPartyMetadata> thirdPartyMetadata;
    ")
    list(LENGTH 3RDPARTY_NAMES 3RDPARTY_NAMES_NUMBER)
    list(LENGTH 3RDPARTY_LICENSES 3RDPARTY_LICENSES_NUMBER)
    list(LENGTH 3RDPARTY_URLS 3RDPARTY_URLS_NUMBER)
    IF(NOT(${3RDPARTY_NAMES_NUMBER} EQUAL 0) AND NOT(${3RDPARTY_LICENSES_NUMBER} EQUAL 0) AND NOT(${3RDPARTY_URLS_NUMBER} EQUAL 0))
        IF(NOT(${3RDPARTY_NAMES_NUMBER} EQUAL ${3RDPARTY_LICENSES_NUMBER}) OR NOT(${3RDPARTY_NAMES_NUMBER} EQUAL ${3RDPARTY_URLS_NUMBER}))
            MESSAGE(FATAL_ERROR "The 3rdparty names/licenses/numbers lists for plugin ${TARGET_NAME} have different sizes, check ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
        ENDIF()
        math(EXPR NUMBER "${3RDPARTY_NAMES_NUMBER} - 1")
        foreach(3RDPARTY RANGE 0 ${NUMBER})
            list(GET 3RDPARTY_NAMES ${3RDPARTY} 3RDPARTY_NAME)
            list(GET 3RDPARTY_LICENSES ${3RDPARTY} 3RDPARTY_LICENSE)
            list(GET 3RDPARTY_URLS ${3RDPARTY} 3RDPARTY_URL)
            file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h "\t\tthirdPartyMetadata.push_back(ACThirdPartyMetadata(\"${3RDPARTY_NAME}\",\"${3RDPARTY_LICENSE}\",\"${3RDPARTY_URL}\"));\n")
        endforeach(3RDPARTY)
    ENDIF()
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h "\treturn thirdPartyMetadata;
    }
    };
    #endif
    ")

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
    IF(WITH_QT)
        #MESSAGE("Qt4-powered MediaCycle plugin")
        FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui ${CMAKE_CURRENT_BINARY_DIR}/*.ui)

        # Generates ui_*.h files
        IF(USE_QT4)
            QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        ELSEIF(USE_QT5)
            QT5_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})
        ENDIF()

        # Generates moc_*.cxx files
        IF(USE_QT4)
            QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDR} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
        ELSEIF(USE_QT5)
            QT5_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDR} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed
        ENDIF()

        # Don't forget to include output directory, otherwise
        # the UI file won't be wrapped!
        include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)
        foreach(MC_MEDIA_LIB ${MC_MEDIA_LIBS})
            include_directories(${CMAKE_BINARY_DIR}/media/${MC_MEDIA_LIB})
        endforeach(MC_MEDIA_LIB)
        ADD_LIBRARY(${LIBRARY_NAME} STATIC ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp)
    ELSE()
        include_directories(${CMAKE_CURRENT_BINARY_DIR})
        ADD_LIBRARY(${LIBRARY_NAME} STATIC ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR} ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.h ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_LIBRARY_NAME}.cpp)
    ENDIF()

    

    IF(NOT NO_SHARED)
        ADD_LIBRARY(${TARGET_NAME} SHARED  ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp)
    ENDIF(NOT NO_SHARED)

    IF(CMAKE_TOOLCHAIN_FILE AND MINGW AND OSG_PLUGINS)
        foreach(OSG_PLUGIN ${OSG_PLUGINS})
            string(REGEX MATCH ".*svg.*" WITH_OSGDB_SVG "${OSG_PLUGIN}")
            string(REGEX MATCH ".*ffmpeg.*" WITH_OSGDB_FFMPEG "${OSG_PLUGIN}")
            IF(WITH_OSGDB_SVG AND WITH_OSGDB_SVG AND RSVG_FOUND)
                TARGET_LINK_LIBRARIES(${LIBRARY_NAME} osgdb_svg)
            ELSEIF(WITH_OSGDB_FFMPEG AND WITH_OSGDB_FFMPEG AND FFMPEG_FOUND)
                TARGET_LINK_LIBRARIES(${LIBRARY_NAME} osgdb_ffmpeg)
            ELSE()
                TARGET_LINK_LIBRARIES(${LIBRARY_NAME} -L${OSG_LIBRARY_DIR}/osgPlugins-${OPENSCENEGRAPH_VERSION} -l${OSG_PLUGIN})
            ENDIF()
        endforeach(OSG_PLUGIN)
    ENDIF()

    IF(WITH_QT)
            #TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-osg-qt)
            TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-qt)

            IF(USE_QT5)
                    qt5_use_modules(${TARGET_NAME} Core Gui Widgets)
                    IF(USE_WEBKIT)
                        qt5_use_modules(${TARGET_NAME} WebKitWidgets)
                    ENDIF(USE_WEBKIT)
                    #set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC ON)
                    #target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
            ENDIF()
    ENDIF()

    foreach(MC_MEDIA_LIB ${MC_MEDIA_LIBS})
        STRING(REGEX REPLACE "mediacycle-" "" MC_MEDIA_LIB "${MC_MEDIA_LIB}")
        IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
            MESSAGE(FATAL_ERROR "mediacycle-${MC_MEDIA_LIB} is not part of the MediaCycle media libs in ${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
        ENDIF()
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB})
        ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle-${MC_MEDIA_LIB})
        TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-${MC_MEDIA_LIB})
    endforeach(MC_MEDIA_LIB)

    foreach(MC_LIB ${MC_LIBS})
        STRING(REGEX REPLACE "mediacycle-" "" MC_LIB "${MC_LIB}")
        IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/${MC_LIB}")
            MESSAGE(FATAL_ERROR "mediacycle-${MC_LIB} is not part of the MediaCycle libs in ${CMAKE_SOURCE_DIR}/libs/${MC_LIB}")
        ENDIF()
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/${MC_LIB})
        IF(NOT TARGET "mediacycle-${MC_LIB}" AND NOT TARGET "${MC_LIB}")
            MESSAGE(FATAL_ERROR "mediacycle-${MC_LIB} is not a target")
        ENDIF()
        IF(TARGET "mediacycle-${MC_LIB}")
            ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle-${MC_LIB})
            TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle-${MC_LIB})
        ENDIF()
        IF(TARGET "${MC_LIB}")
            ADD_DEPENDENCIES(${LIBRARY_NAME} ${MC_LIB})
            TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${MC_LIB})
        ENDIF()
    endforeach(MC_LIB)

    SET(DIRECTORIES_TO_INCLUDE)
    foreach(MC_3RDPARTY ${MC_3RDPARTIES})
        IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
            MESSAGE(FATAL_ERROR "${MC_3RDPARTY} is not part of the MediaCycle 3rd parties in ${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
        ENDIF()
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY})
        ADD_DEPENDENCIES(${LIBRARY_NAME} ${MC_3RDPARTY})
        TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${MC_3RDPARTY})

        GET_TARGET_PROPERTY(MC_3RDPARTY_INCLUDE_DIRECTORIES "${MC_3RDPARTY}" INCLUDE_DIRECTORIES)
        #STRING(REGEX REPLACE "MC_3RDPARTY_INCLUDE_DIRECTORIES-NOTFOUND" "" MC_3RDPARTY_INCLUDE_DIRECTORIES ${MC_3RDPARTY_INCLUDE_DIRECTORIES})
        #MESSAGE("MC_3RDPARTY_INCLUDE_DIRECTORIES ${MC_3RDPARTY_INCLUDE_DIRECTORIES}")
        FOREACH(MC_3RDPARTY_INCLUDE_DIRECTORY ${MC_3RDPARTY_INCLUDE_DIRECTORIES})
            LIST(APPEND DIRECTORIES_TO_INCLUDE "${MC_3RDPARTY_INCLUDE_DIRECTORY}")
        ENDFOREACH(MC_3RDPARTY_INCLUDE_DIRECTORY)
    endforeach(MC_3RDPARTY)
    IF(DIRECTORIES_TO_INCLUDE)
        LIST(REMOVE_DUPLICATES DIRECTORIES_TO_INCLUDE)
        #MESSAGE("DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE}")
        INCLUDE_DIRECTORIES(${DIRECTORIES_TO_INCLUDE})
    ENDIF()

    IF(NOT MC_3RDPARTY AND NOT MC_MEDIA_LIBS)
        ADD_DEPENDENCIES(${LIBRARY_NAME} mediacycle)
        TARGET_LINK_LIBRARIES(${LIBRARY_NAME} mediacycle)
    ENDIF()

    SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES PREFIX ${PLUGIN_PREFIX})
    SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})
    SET_TARGET_PROPERTIES(${LIBRARY_NAME} PROPERTIES FOLDER "${FOLDERNAME}/static")

    IF(NOT NO_SHARED)

        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBRARY_NAME})

        SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PREFIX ${PLUGIN_PREFIX})
        SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})
        SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}/dynamic")

    ENDIF(NOT NO_SHARED)

    IF(CMAKE_CAN_APPEND_PROPERTIES)
        IF(NOT NO_SHARED)
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY MC_3RDPARTIES "${MC_3RDPARTIES}")
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY MC_LIBS "${MC_LIBS}")
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY MC_MEDIA_LIBS "${MC_MEDIA_LIBS}")
        ENDIF(NOT NO_SHARED)
        set_property(TARGET "${LIBRARY_NAME}" APPEND PROPERTY MC_3RDPARTIES "${MC_3RDPARTIES}")
        set_property(TARGET "${LIBRARY_NAME}" APPEND PROPERTY MC_LIBS "${MC_LIBS}")
        set_property(TARGET "${LIBRARY_NAME}" APPEND PROPERTY MC_MEDIA_LIBS "${MC_MEDIA_LIBS}")

    ENDIF()

    #SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES SUFFIX ${PLUGIN_SUFFIX})
    #INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})

endmacro(ADD_MC_PLUGIN_LIBRARY PLUGIN_NAME)
