set(stb_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rd/nothings")

include_directories(${stb_SOURCE_DIR}/include)

set(stb_SOURCES
    "${stb_SOURCE_DIR}/src/stb_vorbis.c"
)

add_library(stb STATIC ${stb_SOURCES})

set_target_properties(stb PROPERTIES
    C_STANDARD 99)