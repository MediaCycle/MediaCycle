#List the files in "usability" and if there are directories, add them to the list of (potential) test programs
 
MESSAGE("\nUsability testing:")
SET(FOLDERNAME "usability")

FILE(GLOB DIRLIST * )

FOREACH(TESTDIR ${DIRLIST})
    IF(IS_DIRECTORY ${TESTDIR})
        ADD_SUBDIRECTORY(${TESTDIR})
    ENDIF()
ENDFOREACH()
