# SparseMatrix for text mining, deprecated

#IF(SUPPORT_TEXT)
#	FILE(GLOB SRC *.cpp *.c)
#	FILE(GLOB HDR *.hpp *.h)
#
#	ADD_LIBRARY(SparseMatrix ${SRC} ${HDR})
#	ADD_DEPENDENCIES(SparseMatrix mediacycle)
#
#	MESSAGE("[X] SparseMatrix")
#ELSE()
	MESSAGE("[ ] SparseMatrix")
#ENDIF()