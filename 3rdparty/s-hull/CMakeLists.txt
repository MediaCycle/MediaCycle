# s-hull from http://www.s-hull.org/
# Beerware -> http://www.s-hull.org/index_files/s_hull_tcs.html
SET(TARGET_NAME "s-hull")
IF(1)
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

        FILE(GLOB ${TARGET_NAME}_SRCS s_hull.C)
        FILE(GLOB ${TARGET_NAME}_HDRS s_hull.H)

        ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})

        IF(CMAKE_CAN_APPEND_PROPERTIES)
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
        ENDIF()
        SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
