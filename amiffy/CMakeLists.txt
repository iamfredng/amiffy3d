link_libraries(
        logc lua
        stb
        "${CMAKE_SOURCE_DIR}/3rd/irrKlang/irrKlang.lib"
)

include_directories(
        "${AMIFFY3D_SOURCE_DIR}/3rd/logc/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/lua/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/nothings/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/irrKlang/include"
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DSOUNDIO_ON_MSVC)
endif ()
set(IMGUI_LIB imgui::imgui)
#set(GLAD_GL "${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include/glad/gl.h")

if (WIN32)
    set(ICON glfw.rc)
endif ()

set(AMIFFY_SOURCE
        amiffy.h main.cpp
        imgui_impl_dx11.h imgui_impl_dx11.cpp
        imgui_impl_win32.h imgui_impl_win32.cpp
        images.h images.c
        #        audio_client.h
        #        audio_client.c
        #        lua_bind.h
        #        lua_bind.c
        #        lua_print_stack.h
        #        lua_print_stack.c stb_img.c stb_img.h
        scripts.h scripts.c
        audio.h audio.c
        ui.h ui.c
        printstack.h printstack.c)

# add_executable(AMIFFY3D ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

add_executable(AMIFFY3D WIN32 MACOSX_BUNDLE ${AMIFFY_SOURCE} ${GLAD_GL} ${ICON})

#target_link_libraries(AMIFFY3D irrKlang)

set_target_properties(AMIFFY3D PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../"
        CXX_STANDARD 20
        C_STANDARD 99)

set(CLANG TRUE)

find_package(DirectX11 REQUIRED)
find_package(imgui CONFIG REQUIRED)

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
