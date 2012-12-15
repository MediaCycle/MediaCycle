#List the files in "ugly" and if there are directories, add them to the list of (potential) test programs
 
MESSAGE("\nUgly tests (hacks):")
SET(FOLDERNAME "tests")

FILE(GLOB DIRLIST * )

FOREACH(TESTDIR ${DIRLIST})
    IF(IS_DIRECTORY ${TESTDIR})
        ADD_SUBDIRECTORY(${TESTDIR})
    ENDIF()
ENDFOREACH()
