add_library(logc ${LIBRARY_BUILD_TYPE} src/log.c)

target_include_directories(logc PRIVATE "include")

set_target_properties(logc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist"
        C_STANDARD 99)
