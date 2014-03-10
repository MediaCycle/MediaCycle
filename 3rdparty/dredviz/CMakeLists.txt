# 1.0.2 released on 3.10.2012 From http://research.ics.aalto.fi/mi/software/dredviz/, LGPL
SET(TARGET_NAME "dredviz")
IF(ARMADILLO_FOUND AND NEWMAT_FOUND)

	ADD_DEFINITIONS(-Duse_namespace)

	INCLUDE_DIRECTORIES( ${NEWMAT_INCLUDE_DIR})

	FILE(GLOB ${TARGET_NAME}_SRCS calculatedistancematrix.cc
		commandlineargs.cc
		conjugategradientopt.cc
		datamatrix.cc
		dataset.cc
		distancematrix.cc
		distancematrixloader.cc
		dynamicdouble.cc
		euclidean.cc
		euclideansquared.cc
		exception.cc
		goldensectionsearch.cc
		inputprobentropy.cc
		nervcostfunction.cc
		nervlib.cc
		nervoptstrat.cc
		nervprobability.cc
		randomdatagenerator.cc
		recorder.cc
		sompackexporter.cc
		sompackloader.cc
	)
        FILE(GLOB ${TARGET_NAME}_HDRS calculatedistancematrix.hh
		commandlineargs.hh
		conjugategradientopt.hh
		datamatrix.hh
		dataset.hh
		distancematrix.hh
		distancematrixloader.hh
		dynamicdouble.hh
		euclidean.hh
		euclideansquared.hh
		exception.hh
		goldensectionsearch.hh
		inputprobentropy.hh
		nerv.hh
		nervcostfunction.hh
		nervlib.hh
		nervoptstrat.hh
		nervprobability.hh
		randomdatagenerator.hh
		recorder.hh
		sompackexporter.hh
		sompackloader.hh
	)

	ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})
        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${NEWMAT_LIBRARIES} ${ARMADILLO_LIBRARIES})

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
