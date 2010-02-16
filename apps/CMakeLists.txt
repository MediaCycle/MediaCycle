#List the files in "apps" and if there are directories, add them to the list of (potential) test programs
MESSAGE("apps")
FILE(GLOB DIRLIST * )

FOREACH(APPDIR ${DIRLIST})
    IF(IS_DIRECTORY ${APPDIR})
        MESSAGE("Possible apps found in : " ${APPDIR})
        ADD_SUBDIRECTORY(${APPDIR})
    ENDIF()
ENDFOREACH()
