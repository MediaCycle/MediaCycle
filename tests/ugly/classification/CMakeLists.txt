IF(TORCH3_FOUND)
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

    INCLUDE_DIRECTORIES(../../../plugins/classification)

	ADD_EXECUTABLE(classification-test ${SRC} ${HDR})

	ADD_DEPENDENCIES(classification-test mc_classification)

	TARGET_LINK_LIBRARIES(classification-test mc_classification ${TORCH3_LIBRARY})

	MESSAGE("[X] classification")
ELSE()
	MESSAGE("[ ] classification")
ENDIF()

