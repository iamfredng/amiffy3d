add_library(logc ${LIBRARY_BUILD_TYPE} src/log.c)

target_include_directories(logc PRIVATE "include")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(logc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

set_target_properties(logc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
        C_STANDARD 99)
