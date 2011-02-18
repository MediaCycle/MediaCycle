#List the files in "test" and if there are directories, add them to the list of (potential) test programs

MESSAGE("\nTests:")

FILE(GLOB DIRLIST * )

FOREACH(TESTDIR ${DIRLIST})
    IF(IS_DIRECTORY ${TESTDIR})
        ADD_SUBDIRECTORY(${TESTDIR})
    ENDIF()
ENDFOREACH()
