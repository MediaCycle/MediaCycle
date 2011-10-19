# Core dependencies
add_subdirectory(fftooura)
IF(NOT TINYXML_FOUND)
	add_subdirectory(tinyxml)
ENDIF()
add_subdirectory(tree)
add_subdirectory(cwaves)

# Optional dependencies
add_subdirectory(gnuplot_i)
IF(USE_VAMP)
add_subdirectory(vamp-plugin-sdk)
ENDIF()

# OSG plugins are added at the end of the project