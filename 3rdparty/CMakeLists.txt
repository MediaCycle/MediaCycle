# Core dependencies
add_subdirectory(fftooura)
add_subdirectory(tinyxml)
add_subdirectory(tree)

# Optional dependencies
add_subdirectory(gnuplot_i)
IF(USE_OSC)
add_subdirectory(oscpack)
ENDIF()
IF(USE_VAMP)
add_subdirectory(vamp-plugin-sdk)
ENDIF()
