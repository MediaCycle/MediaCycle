# Core dependencies
add_subdirectory(fftooura)
add_subdirectory(tinyxml)
add_subdirectory(tree)
add_subdirectory(cwaves)

# Optional dependencies
add_subdirectory(gnuplot_i)
IF(USE_VAMP)
add_subdirectory(vamp-plugin-sdk)
ENDIF()

# OSG plugins are added at the end of the project