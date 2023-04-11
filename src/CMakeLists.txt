link_libraries(glfw)
link_libraries(logc)

include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/nuklear/include")
include_directories("${AMIFFY3D_SOURCE_DIR}/3rd/logc/include")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(GLAD_GL "${AMIFFY3D_SOURCE_DIR}/3rd/glfw3/include/glad/gl.h")

if (WIN32)
    set(ICON glfw.rc)
endif()

add_executable(amiffy3d main.c ${GLAD_GL} ${ICON})

# add_executable(amiffy3d WIN32 MACOSX_BUNDLE main.c ${GLAD_GL} ${ICON})

set_target_properties(amiffy3d PROPERTIES
    C_STANDARD 99)

if (MSVC)
    # Tell MSVC to use main instead of WinMain
    set_target_properties(amiffy3d PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    # Tell Clang using MS CRT to use main instead of WinMain
    set_target_properties(amiffy3d PROPERTIES
                          LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()

target_compile_definitions(amiffy3d PRIVATE UNICODE _UNICODE)