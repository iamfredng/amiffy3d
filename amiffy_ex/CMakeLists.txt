add_library(AMIFFY_EX MODULE
        amiffy_ex.c ${AMIFFY3D_SOURCE_DIR}/amiffy/amiffy.h )

target_link_libraries(AMIFFY_EX
#        PRIVATE logc
        PRIVATE lua)

target_include_directories(AMIFFY_EX
#        PUBLIC "${PROJECT_SOURCE_DIR}/3rd/logc/include"
        PUBLIC "${PROJECT_SOURCE_DIR}/3rd/lua/include")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

set(AMIFFY_EX_VERSION 0.5.0)

message("AMIFFY_EX_VERSION: ${AMIFFY_EX_VERSION}")

set_target_properties(AMIFFY_EX PROPERTIES
        VERSION ${AMIFFY_EX_VERSION}
        OUTPUT_NAME "amiffy_ex"
        #        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../"
        C_STANDARD 99)
SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/clib")
target_compile_definitions(AMIFFY_EX PRIVATE UNICODE _UNICODE)