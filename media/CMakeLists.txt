#List the files in "media" and if there are directories, add them to the list of (potential) media

MESSAGE("\nMedia libraries:")

FILE(GLOB DIRLIST * )

FOREACH(MEDIADIR ${DIRLIST})
    IF(IS_DIRECTORY ${MEDIADIR})
        ADD_SUBDIRECTORY(${MEDIADIR})
    ENDIF()
ENDFOREACH()
