cmake_minimum_required(VERSION 3.4...3.26 FATAL_ERROR)

project(AMIFFY3D VERSION 0.1.1 LANGUAGES C CXX)

# set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (WIN32)
    set(CMAKE_C_FLAGS "-Wno-dll-attribute-on-redeclaration")
endif ()

#include(CMakeDependentOption)

cmake_dependent_option(GLFW_USE_HYBRID_HPG "Force use of high-performance GPU on hybrid systems" OFF
                       "WIN32" OFF)
cmake_dependent_option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON
                       "MSVC" OFF)
cmake_dependent_option(GLFW_BUILD_WIN32 "Build support for Win32" ON "WIN32" OFF)

set(GLFW_LIBRARY_TYPE "${GLFW_LIBRARY_TYPE}" CACHE STRING
    "Library type override for GLFW (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (GLFW_BUILD_WIN32)
    message(STATUS "Including Win32 support")
endif()

set(GLFW_VERSION 3.4.0)

set(LIBRARY_BUILD_TYPE SHARED)

add_subdirectory(3rd/glfw3)
add_subdirectory(3rd/logc)
add_subdirectory(3rd/lua)
#add_subdirectory(3rd/soundio)
add_subdirectory(3rd/nothings)
#add_subdirectory(amiffy_ex)
add_subdirectory(amiffy)
