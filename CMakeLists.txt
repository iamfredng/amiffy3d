cmake_minimum_required(VERSION 3.4...3.26 FATAL_ERROR)

project(AMIFFY3D VERSION 0.1.1 LANGUAGES C)

# if (POLICY CMP65001)
#     cmake_policy(SET CMP65001 NEW)
# endif()

# if (POLICY CMP0069)
#     cmake_policy(SET CMP0069 NEW)
# endif()

# set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

include(CMakeDependentOption)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

cmake_dependent_option(GLFW_USE_HYBRID_HPG "Force use of high-performance GPU on hybrid systems" OFF
                       "WIN32" OFF)
cmake_dependent_option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" ON
                       "MSVC" OFF)
cmake_dependent_option(GLFW_BUILD_WIN32 "Build support for Win32" ON "WIN32" OFF)

set(GLFW_LIBRARY_TYPE "${GLFW_LIBRARY_TYPE}" CACHE STRING
    "Library type override for GLFW (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (GLFW_BUILD_WIN32)
    message(STATUS "Including Win32 support")
endif()

# include(CMakePackageConfigHelpers)

# set(GLFW_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/glfw3")
set(GLFW_VERSION 3.4.0)

# configure_package_config_file(CMake/glfw3Config.cmake.in
#     3rd/glfw3/glfw3Config.cmake
#     INSTALL_DESTINATION "${GLFW_CONFIG_PATH}"
#     NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# write_basic_package_version_file(3rd/glfw3/glfw3ConfigVersion.cmake
#                                  VERSION ${GLFW_VERSION}
#                                  COMPATIBILITY SameMajorVersion)

add_subdirectory(3rd/glfw3)
add_subdirectory(3rd/logc)
add_subdirectory(3rd/lua)
add_subdirectory(3rd/soundio)
add_subdirectory(3rd/nothings)
add_subdirectory(amiffy)

message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")