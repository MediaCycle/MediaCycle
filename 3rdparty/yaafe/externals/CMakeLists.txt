file(GLOB_RECURSE smarc_SOURCES audio-smarc/*.c)
file(GLOB tmpfile audio-smarc/main.c)
list(REMOVE_ITEM smarc_SOURCES ${tmpfile})

add_definitions(-std=c99 -fPIC)
add_library (audio-smarc ${smarc_SOURCES})
	SET_TARGET_PROPERTIES("audio-smarc" PROPERTIES FOLDER "${FOLDERNAME}/yaafe/")


