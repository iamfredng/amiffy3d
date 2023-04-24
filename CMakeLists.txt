cmake_minimum_required(VERSION 3.4...3.26 FATAL_ERROR)

project(AMIFFY3D VERSION 0.1.1 LANGUAGES C CXX)

include(CMakeDependentOption)

# set(CMAKE_BUILD_TYPE Release)
set(CLANG TRUE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

if (WIN32)
    set(CMAKE_C_FLAGS "-Wno-dll-attribute-on-redeclaration")
endif ()

cmake_dependent_option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON
                       "MSVC" OFF)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(LIBRARY_BUILD_TYPE SHARED)

add_subdirectory(3rd/logc)
add_subdirectory(3rd/lua)
add_subdirectory(3rd/nothings)
#add_subdirectory(amiffy_ex)
add_subdirectory(amiffy)
