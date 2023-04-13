link_libraries(glfw)
link_libraries(logc)
link_libraries(lua)
link_libraries(soundio)

include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/nuklear/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/logc/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/lua/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/soundio/include")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DSOUNDIO_ON_MSVC)
endif()

set(GLAD_GL "${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include/glad/gl.h")

if (WIN32)
    set(ICON glfw.rc)
endif()

set(AMIFFY_SOURCE
    amiffy.h
    main.c
    audio_client.h
    audio_client.c
    lua_bind.h
    lua_bind.c
)

# add_executable(AMIFFY3D ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

add_executable(AMIFFY3D WIN32 MACOSX_BUNDLE ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

set_target_properties(AMIFFY3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../"
    C_STANDARD 99)

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(AMIFFY3D PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(AMIFFY3D PROPERTIES
                          LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()

target_compile_definitions(AMIFFY3D PRIVATE UNICODE _UNICODE)
