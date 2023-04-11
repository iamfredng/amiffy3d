add_library(logc STATIC src/log.c)

target_include_directories(logc PRIVATE "include")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(logc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

set_target_properties(logc PROPERTIES
    C_STANDARD 99)