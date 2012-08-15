# Core dependencies
add_subdirectory(fftooura)
IF(NOT TINYXML_FOUND)
	add_subdirectory(tinyxml)
ENDIF()
add_subdirectory(tree)

# Optional dependencies
IF(SUPPORT_AUDIO)
	add_subdirectory(cwaves)
ENDIF()
IF(SUPPORT_AUDIO AND USE_YAAFE)
	add_subdirectory(yaafe)
ENDIF()
add_subdirectory(gnuplot_i)
IF(USE_VAMP)
add_subdirectory(vamp-plugin-sdk)
ENDIF()

IF(OCTAVE_FOUND)
add_subdirectory(octave_embed)
IF(USE_MAKAM)
add_subdirectory(octave_yin)
ENDIF()
ENDIF()

# OSG plugins are added at the end of the project