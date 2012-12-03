IF(OCTAVE_FOUND AND USE_MAKAM)
	FILE(GLOB SRCS *.c)
	FILE(GLOB HDR *.h)

	foreach(SRC ${SRCS})
	GET_FILENAME_COMPONENT(MEX ${SRC} NAME_WE)
	SET(MEXFILE "${CMAKE_CURRENT_BINARY_DIR}/${MEX}.mex")
	SET(MEXFILES "${MEXFILES};${MEXFILE}")
	ADD_CUSTOM_COMMAND(
		OUTPUT "${MEXFILE}"
    		COMMAND ${MKOCTFILE_EXECUTABLE}
		ARGS --mex -o ${MEXFILE} ${SRC}
		DEPENDS "${SRC}"
		COMMENT "Generating ${MEXFILE}"
    	)
	#add_custom_target(octave_yin_${MEX} DEPENDS ${MEXFILE} SOURCES ${SRC}) #uncomment to get target for each oct file
	endforeach(SRC)
	SET(MEX_TARGETS "${MEX_TARGETS} octave_yin_${MEX}")
	add_custom_target(octave_yin DEPENDS ${MEXFILES} SOURCES ${SRCS})

	MESSAGE("[X] yin for octave")
ELSE()
	MESSAGE("[ ] yin for octave")
ENDIF()
