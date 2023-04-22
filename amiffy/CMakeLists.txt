link_libraries(
        glfw logc lua stb
)

include_directories(
        "${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/nuklear/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/logc/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/lua/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/irrklang/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/nothings/include"
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DSOUNDIO_ON_MSVC)
endif ()

set(GLAD_GL "${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include/glad/gl.h")

if (WIN32)
    set(ICON glfw.rc)
endif ()

set(AMIFFY_SOURCE
        amiffy.h main.cpp
        images.h images.c
        #        lua_bind.h
        #        lua_bind.c
        #        lua_print_stack.h
        #        lua_print_stack.c stb_img.c stb_img.h
        scripts.h scripts.c
        audio.h audio.cpp
        ui.h ui.c
        printstack.h printstack.c)

# add_executable(AMIFFY3D ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

add_executable(AMIFFY3D WIN32 MACOSX_BUNDLE ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

target_link_libraries(AMIFFY3D
        PRIVATE ${CMAKE_SOURCE_DIR}/3rd/irrklang/irrKlang.lib)

set_target_properties(AMIFFY3D PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../"
        CXX_STANDARD 20
        C_STANDARD 99)

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(AMIFFY3D PROPERTIES
            LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(AMIFFY3D PROPERTIES
            LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif ()

target_compile_definitions(AMIFFY3D PRIVATE UNICODE _UNICODE)
