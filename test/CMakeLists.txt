#List the files in "test" and if there are directories, add them to the list of (potential) test programs
FILE(GLOB DIRLIST * )

FOREACH(TESTDIR ${DIRLIST})
    IF(IS_DIRECTORY ${TESTDIR})
        MESSAGE("Possible test found in : " ${TESTDIR})
        ADD_SUBDIRECTORY(${TESTDIR})
    ENDIF()
ENDFOREACH()
