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
IF(OSG_FOUND AND FFMPEG_FOUND)
add_subdirectory(osgdb_ffmpeg)
ENDIF()
