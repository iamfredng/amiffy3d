link_libraries(logc lua stb)

include_directories(
        "${AMIFFY3D_SOURCE_DIR}/3rd/logc/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/lua/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/irrklang/include"
        "${AMIFFY3D_SOURCE_DIR}/3rd/nothings/include"
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

if (WIN32)
    set(ICON glfw.rc)
endif ()

find_package(DirectX11 REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(AMIFFY_SOURCE
        amiffyconf.h main.cpp
        images.h images.c
        scripts.h scripts.cpp
        audio.h audio.cpp
        ui.h ui.cpp
        printstack.h printstack.c
        imgui_impl_win32.h imgui_impl_win32.cpp
        imgui_impl_dx11.h imgui_impl_dx11.cpp
        uicomponents.h uicomponents.cpp
        amiffy.cpp amiffy.h)

#add_executable(AMIFFY3D ${AMIFFY_SOURCE} ${ICON})

add_executable(AMIFFY3D WIN32 MACOSX_BUNDLE ${AMIFFY_SOURCE} ${ICON})

target_link_libraries(AMIFFY3D
        PRIVATE ${DirectX11_LIBRARY}
        PRIVATE imgui::imgui)

target_link_libraries(AMIFFY3D
        PRIVATE ${CMAKE_SOURCE_DIR}/3rd/irrklang/irrKlang.lib)

set_target_properties(AMIFFY3D PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
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

add_custom_command(
        TARGET AMIFFY3D POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/3rd/irrklang/irrKlang.dll
        ${CMAKE_SOURCE_DIR}/dist/irrKlang.dll

        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/3rd/irrklang/ikpFlac.dll
        ${CMAKE_SOURCE_DIR}/dist/ikpFlac.dll

        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/3rd/irrklang/ikpMP3.dll
        ${CMAKE_SOURCE_DIR}/dist/ikpMP3.dll

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lua
        ${CMAKE_SOURCE_DIR}/dist/lua

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/clib
        ${CMAKE_SOURCE_DIR}/dist/clib

        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/fonts/harmony.ttf
        ${CMAKE_SOURCE_DIR}/dist/fonts/harmony.ttf

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/audio
        ${CMAKE_SOURCE_DIR}/dist/audio

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_SOURCE_DIR}/dist/assets
)