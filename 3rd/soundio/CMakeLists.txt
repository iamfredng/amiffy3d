set(libsoundio_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rd/soundio")

include_directories(${libsoundio_SOURCE_DIR}/include)
include_directories(${libsoundio_SOURCE_DIR}/include/soundio)

set(LIBSOUNDIO_SOURCES
    "${libsoundio_SOURCE_DIR}/src/soundio.c"
    "${libsoundio_SOURCE_DIR}/src/util.c"
    "${libsoundio_SOURCE_DIR}/src/os.c"
    "${libsoundio_SOURCE_DIR}/src/dummy.c"
    "${libsoundio_SOURCE_DIR}/src/channel_layout.c"
    "${libsoundio_SOURCE_DIR}/src/ring_buffer.c"
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-DSOUNDIO_ON_MSVC)
    target_compile_definitions(logc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_library(soundio STATIC ${LIBSOUNDIO_SOURCES} ${WIN32_ATOM})

set_target_properties(soundio PROPERTIES
    C_STANDARD 99)