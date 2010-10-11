#List the files in "test" and if there are directories, add them to the list of (potential) test programs
FILE(GLOB DIRLIST * )

FOREACH(TESTDIR ${DIRLIST})
    IF(IS_DIRECTORY ${TESTDIR})
        MESSAGE("Building test: " ${TESTDIR})
        ADD_SUBDIRECTORY(${TESTDIR})
    ENDIF()
ENDFOREACH()
