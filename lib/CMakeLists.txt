#List the files in "plugins" and if there are directories, add them to the list of (potential) plugins
FILE(GLOB DIRLIST * )

FOREACH(LIBDIR ${DIRLIST})
    IF(IS_DIRECTORY ${LIBDIR})
        MESSAGE("Possible library found in : " ${LIBDIR})
        ADD_SUBDIRECTORY(${LIBDIR})
    ENDIF()
ENDFOREACH()
