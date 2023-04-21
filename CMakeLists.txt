cmake_minimum_required(VERSION 3.4...3.26 FATAL_ERROR)

project(AMIFFY3D VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
# set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

include(CMakeDependentOption)

cmake_dependent_option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON
                       "MSVC" OFF)

add_subdirectory(3rd/logc)
add_subdirectory(3rd/lua)
add_subdirectory(3rd/nothings)
#add_subdirectory(3rd/irrklang)
add_subdirectory(amiffy_ex)
add_subdirectory(amiffy)
