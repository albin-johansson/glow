project(glow-src CXX)

# Glow target names
set(GLOW_CORE_TARGET glow-core)
set(GLOW_EDITOR_TARGET glow-editor)
set(GLOW_VULKAN_RHI_TARGET glow-vulkan-rhi)
set(GLOW_OPENGL_RHI_TARGET glow-opengl-rhi)

set(GLOW_PRECOMPILED_HEADERS
    <algorithm>
    <array>
    <charconv>
    <chrono>
    <codecvt>
    <concepts>
    <cstddef>
    <cstdint>
    <cstring>
    <cstdlib>
    <ctime>
    <cmath>
    <deque>
    <exception>
    <filesystem>
    <fstream>
    <functional>
    <iterator>
    <limits>
    <locale>
    <map>
    <memory>
    <numbers>
    <optional>
    <ostream>
    <queue>
    <random>
    <span>
    <sstream>
    <string>
    <string_view>
    <system_error>
    <type_traits>
    <unordered_map>
    <utility>
    <variant>
    <vector>
    )

# Configures properties, compile options, precompiled headers, etc.
function(glow_prepare_target target is_plugin)
  set_target_properties(${target}
                        PROPERTIES
                        CXX_STANDARD 23
                        CXX_STANDARD_REQUIRED ON
                        CXX_EXTENSIONS OFF
                        INTERPROCEDURAL_OPTIMIZATION ${GLOW_ENABLE_LTO}
                        RUNTIME_OUTPUT_DIRECTORY ${GLOW_BUILD_OUTPUT_DIR}
                        LIBRARY_OUTPUT_DIRECTORY ${GLOW_BUILD_OUTPUT_DIR}
                        ARCHIVE_OUTPUT_DIRECTORY ${GLOW_BUILD_OUTPUT_DIR}
                        )

  if (${is_plugin} MATCHES TRUE)
    set_target_properties(${target}
                          PROPERTIES
                          LIBRARY_OUTPUT_DIRECTORY "${GLOW_BUILD_OUTPUT_DIR}/plugins"
                          )
  endif ()

  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${target}
                           PRIVATE
                           /EHsc
                           /MP
                           /W4
                           /bigobj
                           /permissive-
                           /Zc:preprocessor
                           /Zc:__cplusplus
                           )

  else ()
    target_compile_options(${target}
                           PRIVATE
                           -fvisibility=hidden
                           -Wall
                           -Wextra
                           -Wpedantic
                           -Wconversion
                           -Wsign-conversion
                           )
  endif ()

  if (GLOW_VULKAN_SUBSET MATCHES ON)
    target_compile_definitions(${target} PRIVATE GLOW_VULKAN_SUBSET)
  endif ()

  target_include_directories(${target} PRIVATE ${GLOW_INCLUDE_DIR})

  target_precompile_headers(${target} PRIVATE ${GLOW_PRECOMPILED_HEADERS})
endfunction()

add_subdirectory(private/core)
add_subdirectory(private/opengl-rhi)
add_subdirectory(private/vulkan-rhi)
