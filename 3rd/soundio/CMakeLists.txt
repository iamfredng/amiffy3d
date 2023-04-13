set(libsoundio_SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rd/soundio")
set(libsoundio_BINARY_DIR "${CMAKE_SOURCE_DIR}/3rd/soundio")

set(CMAKE_MODULE_PATH ${libsoundio_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories(${libsoundio_SOURCE_DIR}/include)
include_directories(${libsoundio_SOURCE_DIR}/include/soundio)

set(LIBSOUNDIO_VERSION_MAJOR 2)
set(LIBSOUNDIO_VERSION_MINOR 0)
set(LIBSOUNDIO_VERSION_PATCH 0)
set(LIBSOUNDIO_VERSION "${LIBSOUNDIO_VERSION_MAJOR}.${LIBSOUNDIO_VERSION_MINOR}.${LIBSOUNDIO_VERSION_PATCH}")
message("Configuring libsoundio version ${LIBSOUNDIO_VERSION}")

option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_DYNAMIC_LIBS "Build dynamic libraries" ON)
option(BUILD_EXAMPLE_PROGRAMS "Build example programs" ON)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_JACK "Enable JACK backend" ON)
option(ENABLE_PULSEAUDIO "Enable PulseAudio backend" ON)
option(ENABLE_ALSA "Enable ALSA backend" ON)
option(ENABLE_COREAUDIO "Enable CoreAudio backend" ON)
option(ENABLE_WASAPI "Enable WASAPI backend" ON)

find_package(Threads)
if(Threads_FOUND)
    set(STATUS_THREADS "OK")
else(Threads_FOUND)
    set(STATUS_THREADS "not found")
endif(Threads_FOUND)

if(ENABLE_JACK)
    find_package(JACK)
    if(JACK_FOUND)
        set(STATUS_JACK "OK")
        set(SOUNDIO_HAVE_JACK true)
        include_directories(${JACK_INCLUDE_DIR})
    else()
        set(STATUS_JACK "not found")
        set(SOUNDIO_HAVE_JACK false)
        set(JACK_LIBRARY "")
    endif()
else()
    set(STATUS_JACK "disabled")
    set(SOUNDIO_HAVE_JACK false)
    set(JACK_LIBRARY "")
endif()

if(ENABLE_PULSEAUDIO)
    find_package(PulseAudio)
    if(PULSEAUDIO_FOUND)
        set(STATUS_PULSEAUDIO "OK")
        set(SOUNDIO_HAVE_PULSEAUDIO true)
        include_directories(${PULSEAUDIO_INCLUDE_DIR})
    else()
        set(STATUS_PULSEAUDIO "not found")
        set(SOUNDIO_HAVE_PULSEAUDIO false)
        set(PULSEAUDIO_LIBRARY "")
    endif()
else()
    set(STATUS_PULSEAUDIO "disabled")
    set(SOUNDIO_HAVE_PULSEAUDIO false)
    set(PULSEAUDIO_LIBRARY "")
endif()

if(ENABLE_ALSA)
    find_package(ALSA)
    if(ALSA_FOUND)
        set(STATUS_ALSA "OK")
        set(SOUNDIO_HAVE_ALSA true)
        include_directories(${ALSA_INCLUDE_DIRS})
    else()
        set(STATUS_ALSA "not found")
        set(SOUNDIO_HAVE_ALSA false)
        set(ALSA_LIBRARIES "")
    endif()
else()
    set(STATUS_ALSA "disabled")
    set(SOUNDIO_HAVE_ALSA false)
    set(ALSA_LIBRARIES "")
endif()

if(ENABLE_COREAUDIO)
    find_package(CoreAudio)
    if(COREAUDIO_FOUND)
        set(STATUS_COREAUDIO "OK")
        set(SOUNDIO_HAVE_COREAUDIO true)
        include_directories(${COREAUDIO_INCLUDE_DIR})

        find_path(COREFOUNDATION_INCLUDE_DIR NAMES CoreFoundation.h)
        find_library(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
        include_directories(${COREFOUNDATION_INCLUDE_DIR})

        find_path(AUDIOUNIT_INCLUDE_DIR NAMES AudioUnit.h)
        find_library(AUDIOUNIT_LIBRARY NAMES AudioUnit)
        include_directories(${AUDIOUNIT_INCLUDE_DIR})
    else()
        set(STATUS_COREAUDIO "not found")
        set(SOUNDIO_HAVE_COREAUDIO false)
        set(COREAUDIO_LIBRARY "")
        set(COREFOUNDATION_LIBRARY "")
        set(AUDIOUNIT_LIBRARY "")
    endif()
else()
    set(STATUS_COREAUDIO "disabled")
    set(SOUNDIO_HAVE_COREAUDIO false)
    set(COREAUDIO_LIBRARY "")
    set(COREFOUNDATION_LIBRARY "")
    set(AUDIOUNIT_LIBRARY "")
endif()

if(ENABLE_WASAPI)
    find_package(WASAPI)
    if(WASAPI_FOUND)
        set(STATUS_WASAPI "OK")
        set(SOUNDIO_HAVE_WASAPI true)
    else()
        set(STATUS_WASAPI "not found")
        set(SOUNDIO_HAVE_WASAPI false)
    endif()
else()
    set(STATUS_WASAPI "disabled")
    set(SOUNDIO_HAVE_WASAPI false)
endif()


set(LIBSOUNDIO_SOURCES
    "${libsoundio_SOURCE_DIR}/src/soundio.c"
    "${libsoundio_SOURCE_DIR}/src/util.c"
    "${libsoundio_SOURCE_DIR}/src/os.c"
    "${libsoundio_SOURCE_DIR}/src/dummy.c"
    "${libsoundio_SOURCE_DIR}/src/channel_layout.c"
    "${libsoundio_SOURCE_DIR}/src/ring_buffer.c"
)

set(CONFIGURE_OUT_FILE "${libsoundio_BINARY_DIR}/include/soundio/config.h")
configure_file(
    "${libsoundio_SOURCE_DIR}/src/config.h.in"
    ${CONFIGURE_OUT_FILE}
)

if(SOUNDIO_HAVE_JACK)
    set(LIBSOUNDIO_SOURCES ${LIBSOUNDIO_SOURCES}
        "${libsoundio_SOURCE_DIR}/src/jack.c"
    )
endif()
if(SOUNDIO_HAVE_PULSEAUDIO)
    set(LIBSOUNDIO_SOURCES ${LIBSOUNDIO_SOURCES}
        "${libsoundio_SOURCE_DIR}/src/pulseaudio.c"
    )
endif()
if(SOUNDIO_HAVE_ALSA)
    set(LIBSOUNDIO_SOURCES ${LIBSOUNDIO_SOURCES}
        "${libsoundio_SOURCE_DIR}/src/alsa.c"
    )
endif()
if(SOUNDIO_HAVE_COREAUDIO)
    set(LIBSOUNDIO_SOURCES ${LIBSOUNDIO_SOURCES}
        "${libsoundio_SOURCE_DIR}/src/coreaudio.c"
    )
endif()
if(SOUNDIO_HAVE_WASAPI)
    set(LIBSOUNDIO_SOURCES ${LIBSOUNDIO_SOURCES}
        "${libsoundio_SOURCE_DIR}/src/wasapi.c"
    )
endif()

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-DSOUNDIO_ON_MSVC)
    target_compile_definitions(logc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_library(soundio STATIC ${LIBSOUNDIO_SOURCES} ${WIN32_ATOM})

set_target_properties(soundio PROPERTIES
    C_STANDARD 99)

message(
    "System Dependencies\n"
    "-------------------\n"
    "* threads                      : ${STATUS_THREADS}\n"
    "* JACK       (optional)        : ${STATUS_JACK}\n"
    "* PulseAudio (optional)        : ${STATUS_PULSEAUDIO}\n"
    "* ALSA       (optional)        : ${STATUS_ALSA}\n"
    "* CoreAudio  (optional)        : ${STATUS_COREAUDIO}\n"
    "* WASAPI     (optional)        : ${STATUS_WASAPI}\n"
)
