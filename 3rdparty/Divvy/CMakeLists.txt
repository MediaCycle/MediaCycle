# Divvy methods from
# https://github.com/jmlewis/divvy/blob/master/Plugins/Clusterers/KMeans/
# https://github.com/jmlewis/divvy/blob/master/Plugins/Reducers/PCA/
# https://github.com/jmlewis/divvy/blob/master/Plugins/Reducers/TSNE/
# New BSD License
SET(TARGET_NAME "Divvy")
IF(LIBDISPATCH_FOUND)
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

        FILE(GLOB ${TARGET_NAME}_SRCS *.c)
        FILE(GLOB ${TARGET_NAME}_HDRS *.h)

        ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})
	TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LIBDISPATCH_LIBRARY})

        IF(CMAKE_CAN_APPEND_PROPERTIES)
            set_property(TARGET "${TARGET_NAME}" APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
        ENDIF()
        SET_TARGET_PROPERTIES("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
