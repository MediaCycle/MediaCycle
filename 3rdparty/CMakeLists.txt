# Core dependencies
add_subdirectory(fftooura)
IF(NOT TINYXML_FOUND)
	add_subdirectory(tinyxml)
ENDIF()
add_subdirectory(tree)

# Optional dependencies
IF(SUPPORT_AUDIO AND USE_AUDIOFEAT)
	add_subdirectory(cwaves)
ENDIF()
add_subdirectory(gnuplot_i)
IF(USE_VAMP)
add_subdirectory(vamp-plugin-sdk)
ENDIF()

# OSG plugins are added at the end of the project