#List the files in "plugins" and if there are directories, add them to the list of (potential) plugins
FILE(GLOB DIRLIST * )

FOREACH(PLUGINDIR ${DIRLIST})
    IF(IS_DIRECTORY ${PLUGINDIR})
        MESSAGE("Possible plugin found in : " ${PLUGINDIR})
        ADD_SUBDIRECTORY(${PLUGINDIR})
    ENDIF()
ENDFOREACH()
