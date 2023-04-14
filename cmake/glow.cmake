
function(glow_copy_directory_post_build target from to)
  add_custom_command(TARGET ${target} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                     ${from}
                     ${to})
endfunction()

function(glow_symlink_directory_post_build target from to)
  add_custom_command(TARGET ${target} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E create_symlink
                     ${from}
                     ${to})
endfunction()

set(GLOW_COMPILE_DEFINITIONS
    BOOST_ENABLE_ASSERT_DEBUG_HANDLER
    BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
    GLM_ENABLE_EXPERIMENTAL
    IMGUI_DEFINE_MATH_OPERATORS
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    NOMINMAX
    SPDLOG_FMT_EXTERNAL
    WIN32_LEAN_AND_MEAN)

# The Intel macOS GitHub runners seemingly don't provide some of
# the required Vulkan subset definitions, so we condition on the
# presence of an Apple Silicon processor.
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND
    CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
list(APPEND GLOW_COMPILE_DEFINITIONS GRAVEL_USE_VULKAN_SUBSET)
endif ()

function(glow_configure_compile_options target)
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
                           -Wall
                           -Wextra
                           -Wpedantic
                           -Wconversion
                           -Wsign-conversion
                           )
  endif ()

  target_compile_definitions(${target} PRIVATE ${GLOW_COMPILE_DEFINITIONS})
endfunction()

set(GLOW_STANDARD_CPP_HEADERS
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

set(GLOW_PRECOMPILED_HEADERS "${GLOW_STANDARD_CPP_HEADERS}")

set(IMGUI_SOURCES
    "${GLOW_LIB_DIR}/imgui/imconfig.h"
    "${GLOW_LIB_DIR}/imgui/imgui.cpp"
    "${GLOW_LIB_DIR}/imgui/imgui.h"
    "${GLOW_LIB_DIR}/imgui/imgui_demo.cpp"
    "${GLOW_LIB_DIR}/imgui/imgui_draw.cpp"
    "${GLOW_LIB_DIR}/imgui/imgui_internal.h"
    "${GLOW_LIB_DIR}/imgui/imgui_tables.cpp"
    "${GLOW_LIB_DIR}/imgui/imgui_widgets.cpp"
    "${GLOW_LIB_DIR}/imgui/imstb_rectpack.h"
    "${GLOW_LIB_DIR}/imgui/imstb_textedit.h"
    "${GLOW_LIB_DIR}/imgui/imstb_truetype.h"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_sdl2.cpp"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_sdl2.h"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_opengl3.h"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
    "${GLOW_LIB_DIR}/imgui/backends/imgui_impl_vulkan.h"
    )

add_library(imgui "${IMGUI_SOURCES}")

target_compile_definitions(imgui PRIVATE ${GLOW_COMPILE_DEFINITIONS})

target_include_directories(imgui
                           SYSTEM PUBLIC
                           "${GLOW_LIB_DIR}/imgui"
                           "${GLOW_LIB_DIR}/imgui/backends"
                           )

target_link_libraries(imgui
                      PUBLIC
                      SDL2::SDL2
                      Vulkan::Vulkan
                      )