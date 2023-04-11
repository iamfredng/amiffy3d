add_library(glfw ${GLFW_LIBRARY_TYPE}
    # include/GLFW/glfw3.h
    # include/GLFW/glfw3native.h
    src/internal.h src/platform.h src/mappings.h
    src/context.c src/init.c src/input.c src/monitor.c src/platform.c src/vulkan.c src/window.c
    src/egl_context.c src/osmesa_context.c src/null_platform.h src/null_joystick.h
    src/null_init.c src/null_monitor.c src/null_window.c src/null_joystick.c
)

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(glfw PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    target_sources(glfw PRIVATE
        src/win32_time.h src/win32_thread.h src/win32_module.c src/win32_time.c src/win32_thread.c)
endif()

if (GLFW_BUILD_WIN32)
    target_compile_definitions(glfw PRIVATE _GLFW_WIN32)
    target_sources(glfw PRIVATE
        src/win32_platform.h src/win32_joystick.h src/win32_init.c
        src/win32_joystick.c src/win32_monitor.c src/win32_window.c
        src/wgl_context.c)
endif()

set_target_properties(glfw PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 99
    C_EXTENSIONS OFF)

# target_compile_definitions(glfw INTERFACE GLFW_DLL)
