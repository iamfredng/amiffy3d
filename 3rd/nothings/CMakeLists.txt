set(stb_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rd/nothings")

set(stb_SOURCES
    "${stb_SOURCE_DIR}/src/stb_vorbis.c"
)

add_library(stb STATIC ${stb_SOURCES})

set_target_properties(soundio PROPERTIES
    C_STANDARD 99)