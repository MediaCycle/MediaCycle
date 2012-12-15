#List the files in "plugins" and if there are directories, add them to the list of (potential) plugins

MESSAGE("\nPlugins:")

FILE(GLOB DIRLIST * )

SET(FOLDERNAME "plugins")

FOREACH(PLUGINDIR ${DIRLIST})
    IF(IS_DIRECTORY ${PLUGINDIR})
        ADD_SUBDIRECTORY(${PLUGINDIR})
    ENDIF()
ENDFOREACH()

    
