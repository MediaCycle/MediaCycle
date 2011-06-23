
#	MESSAGE("Building textTool ")
	FILE(GLOB SRC *.cpp *.c)
	FILE(GLOB HDR *.hpp *.h)

	ADD_LIBRARY(SparseMatrix STATIC ${SRC} ${HDR})
	ADD_DEPENDENCIES(SparseMatrix mediacycle)

	MESSAGE("[X] SparseMatrix")