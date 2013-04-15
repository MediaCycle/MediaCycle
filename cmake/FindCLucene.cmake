#
# This module looks for clucene (http://clucene.sf.net) support
# It will define the following values
#
# CLUCENE_INCLUDE_DIR  = where CLucene/StdHeader.h can be found
# CLUCENE_LIBRARY_DIR  = where CLucene/clucene-config.h can be found
# CLUCENE_LIBRARY      = the library to link against CLucene
# CLUCENE_LIBRARIES      = the library to link against CLucene
# CLUCENE_VERSION      = The CLucene version string
# CLUCENE_FOUND        = set to 1 if clucene is found
#

MESSAGE ( "Trying to find CLucene" )

FIND_PATH(CLUCENE_INCLUDE_DIR CLucene.h)

SET(CLUCENE_NAMES ${CLUCENE_NAMES} libclucene)
SET(CLUCENE_NAMES ${CLUCENE_NAMES} clucene)
SET(CLUCENE_NAMES ${CLUCENE_NAMES} clucene-core)

FIND_LIBRARY(CLUCENE_LIBRARY NAMES ${CLUCENE_NAMES})

SET(CLUCENE_NAMES_SHARED ${CLUCENE_NAMES_SHARED} clucene-shared)

FIND_LIBRARY(CLUCENE_LIBRARY_SHARED NAMES ${CLUCENE_NAMES_SHARED})

# handle the QUIETLY and REQUIRED arguments and set ARMADILLO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLUCENE DEFAULT_MSG CLUCENE_LIBRARY CLUCENE_INCLUDE_DIR)


IF(CLUCENE_FOUND)
IF(CLUCENE_LIBRARY_SHARED)
  SET(CLUCENE_LIBRARIES ${CLUCENE_LIBRARY} ${CLUCENE_LIBRARY_SHARED})
ELSE()
  SET(CLUCENE_LIBRARIES ${CLUCENE_LIBRARY})
ENDIF()
ENDIF(CLUCENE_FOUND)

IF(EXISTS ${CLUCENE_INCLUDE_DIR}/CLucene/CLuceneConfig.cmake)
	include(${CLUCENE_INCLUDE_DIR}/CLucene/CLuceneConfig.cmake)
	IF(CLUCENE_VERSION_MAJOR)
		MESSAGE("Found CLucene version: ${CLUCENE_VERSION}")
		IF(${CLUCENE_VERSION_MAJOR} LESS 1)
			MESSAGE("Found CLucene version < 1.x: using OLD_CLUCENE definition.")
			ADD_DEFINITIONS(-DOLD_CLUCENE)
		ENDIF()
	ELSE()
		MESSAGE("Couldn't find CLucene version in CLuceneConfig.cmake, assuming older than 2.x: using OLD_CLUCENE definition.")
		ADD_DEFINITIONS(-DOLD_CLUCENE)
	ENDIF()
ELSE()
	MESSAGE("Couldn't find CLucene version through CMake, assuming older than 2.x: using OLD_CLUCENE definition.")
	ADD_DEFINITIONS(-DOLD_CLUCENE)
ENDIF()

SET(CLUCENE_FOUND "NO")
IF(CLUCENE_LIBRARY AND CLUCENE_INCLUDE_DIR)
    SET(CLUCENE_FOUND "YES")
ENDIF(CLUCENE_LIBRARY AND CLUCENE_INCLUDE_DIR)
