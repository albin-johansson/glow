cmake_minimum_required(VERSION 3.24)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  cmake_path(SET VCPKG_ROOT NORMALIZE $ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

project(glow LANGUAGES CXX)

# Build options
option(GLOW_ENABLE_LTO "Enable link-time optimization" OFF)
option(GLOW_VULKAN_SUBSET "Don't require a complete Vulkan implementation" OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(GLOW_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/public")
set(GLOW_ASSET_DIR "${PROJECT_SOURCE_DIR}/assets")
set(GLOW_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

set(GLOW_DEBUG_BUILD_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/build/debug")
set(GLOW_RELEASE_BUILD_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/build/release")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(GLOW_BUILD_OUTPUT_DIR "${GLOW_DEBUG_BUILD_OUTPUT_DIR}")
elseif (CMAKE_BUILD_TYPE MATCHES "Release")
  set(GLOW_BUILD_OUTPUT_DIR "${GLOW_RELEASE_BUILD_OUTPUT_DIR}")
else ()
  message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
endif ()

message(DEBUG "GLOW_ENABLE_LTO: ${GLOW_ENABLE_LTO}")
message(DEBUG "GLOW_VULKAN_SUBSET: ${GLOW_VULKAN_SUBSET}")
message(DEBUG "GLOW_BUILD_OUTPUT_DIR: ${GLOW_BUILD_OUTPUT_DIR}")
message(DEBUG "GLOW_INCLUDE_DIR: ${GLOW_INCLUDE_DIR}")
message(DEBUG "GLOW_LIB_DIR: ${GLOW_LIB_DIR}")
message(DEBUG "GLOW_ASSET_DIR: ${GLOW_ASSET_DIR}")

include(cmake/glow.cmake)

# System dependencies
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

set(VULKAN_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")

# Vcpkg dependencies
find_package(argparse CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

add_subdirectory(src)
