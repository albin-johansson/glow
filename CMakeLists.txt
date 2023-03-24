cmake_minimum_required(VERSION 3.24)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

project(gravel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Enabling IPO...")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif ()

set(GRAVEL_ASSET_DIR "${PROJECT_SOURCE_DIR}/assets")
set(GRAVEL_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

include(cmake/gravel.cmake)

# System dependencies
find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED) # Makes sure Vulkan is available, but we do not use the associated target directly

# Vcpkg dependencies
find_package(assimp CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

add_subdirectory(src/core)
add_subdirectory(src/app)
