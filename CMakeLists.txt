cmake_minimum_required(VERSION 3.24)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

project(gravel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Enabling IPO...")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif ()

include(cmake/gravel.cmake)

# System dependencies
find_package(OpenGL REQUIRED)

