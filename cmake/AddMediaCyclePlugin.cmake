# Optional variables to be set:
#  MC_3RDPARTIES (list): example SET(MC_3RDPARTIES "cvBlobsLib;cvgabor")
#  MC_MEDIA_LIBS (list): example SET(MC_MEDIA_LIBS "image-core;image-features"), without their prefix mediacycle-
#  3RDPARTY_NAMES (list): example list(APPEND 3RDPARTY_NAMES "cvBlobsLib")
#  3RDPARTY_LICENSES (list): example list(APPEND 3RDPARTY_LICENSES "free")
#  3RDPARTY_URLS (list): example list(APPEND 3RDPARTY_URLS "http://opencv.willowgarage.com/wiki/cvBlobsLib")

macro(ADD_MC_PLUGIN_LIBRARY PLUGIN_NAME)

SET(TARGET_NAME "${PLUGIN_PREFIX}${PLUGIN_NAME}")
SET(MEDIA_TYPE "${PLUGIN_NAME}")

FILE(GLOB ${TARGET_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)
FILE(GLOB ${TARGET_NAME}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hh ${CMAKE_CURRENT_BINARY_DIR}/*.h)

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
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/factory.cpp 
"/*
 *  ${TARGET_NAME} factory.cpp
 *  MediaCycle
 *
 *  Don't modify this file, rather its generator:
 *  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
 */

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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
IF(WITH_QT4)
    MESSAGE("Qt4-powered MediaCycle plugin")
    FILE(GLOB ${TARGET_NAME}_UIS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui ${CMAKE_CURRENT_BINARY_DIR}/*.ui)
    # Generates ui_*.h files
    QT4_WRAP_UI(${TARGET_NAME}_UIS_H ${${TARGET_NAME}_UIS})

    # Generates moc_*.cxx files
    QT4_WRAP_CPP(${TARGET_NAME}_MOC_SRCS ${${TARGET_NAME}_HDR} OPTIONS "-nw") # for all headers that potentially declare Q_OBJECT, otherwise warnings are suppressed

    # Don't forget to include output directory, otherwise
    # the UI file won't be wrapped!
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR}/libs/gui-osg-qt)
    foreach(MC_MEDIA_LIB ${MC_MEDIA_LIBS})
        include_directories(${CMAKE_BINARY_DIR}/media/${MC_MEDIA_LIB})
    endforeach(MC_MEDIA_LIB)

    ADD_LIBRARY(${TARGET_NAME} SHARED ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR} ${${TARGET_NAME}_UIS_H} ${${TARGET_NAME}_MOC_SRCS})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-osg-qt)
ELSE()
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    ADD_LIBRARY(${TARGET_NAME} SHARED ${${TARGET_NAME}_SRC} ${${TARGET_NAME}_HDR})
ENDIF()

foreach(MC_3RDPARTY ${MC_3RDPARTIES})
    IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
        MESSAGE(FATAL_ERROR "${MC_3RDPARTY} is not part of the MediaCycle 3rd parties in ${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY}")
    ENDIF()
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/${MC_3RDPARTY})
    ADD_DEPENDENCIES(${TARGET_NAME} ${MC_3RDPARTY})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${MC_3RDPARTY})
endforeach(MC_3RDPARTY)
foreach(MC_MEDIA_LIB ${MC_MEDIA_LIBS})
    STRING(REGEX REPLACE "mediacycle-" "" MC_MEDIA_LIB "${MC_MEDIA_LIB}")
    IF(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
        MESSAGE(FATAL_ERROR "mediacycle-${MC_MEDIA_LIB} is not part of the MediaCycle media libs in ${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB}")
    ENDIF()
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/media/${MC_MEDIA_LIB})
    ADD_DEPENDENCIES(${TARGET_NAME} mediacycle-${MC_MEDIA_LIB})
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle-${MC_MEDIA_LIB})
endforeach(MC_MEDIA_LIB)

IF(NOT MC_3RDPARTY AND NOT MC_MEDIA_LIBS)
    ADD_DEPENDENCIES(${TARGET_NAME} mediacycle)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} mediacycle)
ENDIF()

SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES PREFIX ${PLUGIN_PREFIX})
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PLUGIN_NAME})
#SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES SUFFIX ${PLUGIN_SUFFIX})
#INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib/${PROJECT_NAME})

endmacro(ADD_MC_PLUGIN_LIBRARY PLUGIN_NAME)
