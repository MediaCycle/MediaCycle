# 0.9.0, released on 10.08.2012 from http://research.ics.aalto.fi/mi/software/LinearTimeNeRV/, LGPL
SET(TARGET_NAME "lineartimenerv")
IF(ARMADILLO_FOUND AND NEWMAT_FOUND)

	ADD_DEFINITIONS(-Duse_namespace)

	INCLUDE_DIRECTORIES( ${NEWMAT_INCLUDE_DIR})

	FILE(GLOB ${TARGET_NAME}_SRCS sompackloader.cc dataset.cc distancematrix.cc datamatrix.cc sompackexporter.cc sompackloader.cc randomdatagenerator.cc euclidean.cc commandlineargs.cc geanervcostfunction.cc nervcostfunction.cc geanervoptstrat.cc nervoptstrat.cc conjugategradientopt.cc linearnervlib.cc clustersandexacts.cc exception.cc goldensectionsearch.cc dynamicdouble.cc nervprobability.cc recorder.cc euclideansquared.cc inputprobentropy.cc calculatedistancematrix.cc)
        FILE(GLOB ${TARGET_NAME}_HDRS sompackloader.hh dataset.hh distancematrix.hh datamatrix.hh sompackexporter.hh sompackloader.hh randomdatagenerator.hh euclidean.hh commandlineargs.hh geanervcostfunction.hh nervcostfunction.hh geanervoptstrat.hh nervoptstrat.hh conjugategradientopt.hh linearnervlib.hh pca.hh exception.hh goldensectionsearch.hh dynamicdouble.hh nervprobability.hh recorder.hh euclideansquared.hh inputprobentropy.hh calculatedistancematrix.hh)

	ADD_LIBRARY(${TARGET_NAME} ${${TARGET_NAME}_SRCS} ${${TARGET_NAME}_HDRS})
        TARGET_LINK_LIBRARIES(${TARGET_NAME} ${NEWMAT_LIBRARIES} ${ARMADILLO_LIBRARIES})

	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES FOLDER "${FOLDERNAME}")
	MESSAGE("[X] ${TARGET_NAME}")
ELSE()
	MESSAGE("[ ] ${TARGET_NAME}")
ENDIF()
