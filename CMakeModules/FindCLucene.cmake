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

FIND_LIBRARY(CLUCENE_LIBRARY
        NAMES ${CLUCENE_NAMES}
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment]/lib
        /usr/freeware/lib64
    )


SET(CLUCENE_NAMES_SHARED ${CLUCENE_NAMES_SHARED} clucene-shared)

FIND_LIBRARY(CLUCENE_LIBRARY_SHARED
        NAMES ${CLUCENE_NAMES_SHARED}
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        [HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session\ Manager\\Environment]/lib
        /usr/freeware/lib64
    )

# handle the QUIETLY and REQUIRED arguments and set ARMADILLO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLUCENE DEFAULT_MSG CLUCENE_LIBRARY CLUCENE_INCLUDE_DIR)


IF(CLUCENE_FOUND)
  SET(CLUCENE_LIBRARIES ${CLUCENE_LIBRARY})
  SET(CLUCENE_LIBRARIES ${CLUCENE_LIBRARY} ${CLUCENE_LIBRARY_SHARED})
ENDIF(CLUCENE_FOUND)

SET(CLUCENE_FOUND "NO")
IF(CLUCENE_LIBRARY AND CLUCENE_INCLUDE_DIR)
    SET(CLUCENE_FOUND "YES")
ENDIF(CLUCENE_LIBRARY AND CLUCENE_INCLUDE_DIR)