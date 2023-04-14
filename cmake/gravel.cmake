
function(gravel_copy_directory_post_build target from to)
  add_custom_command(TARGET ${target} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                     ${from}
                     ${to})
endfunction()

function(gravel_symlink_directory_post_build target from to)
  add_custom_command(TARGET ${target} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E create_symlink
                     ${from}
                     ${to})
endfunction()

set(GRAVEL_COMPILE_DEFINITIONS
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
list(APPEND GRAVEL_COMPILE_DEFINITIONS GRAVEL_USE_VULKAN_SUBSET)
endif ()

function(gravel_configure_compile_options target)
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

  target_compile_definitions(${target} PRIVATE ${GRAVEL_COMPILE_DEFINITIONS})
endfunction()

set(GRAVEL_STANDARD_CPP_HEADERS
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

set(GRAVEL_PRECOMPILED_HEADERS "${GRAVEL_STANDARD_CPP_HEADERS}")

set(IMGUI_SOURCES
    "${GRAVEL_LIB_DIR}/imgui/imconfig.h"
    "${GRAVEL_LIB_DIR}/imgui/imgui.cpp"
    "${GRAVEL_LIB_DIR}/imgui/imgui.h"
    "${GRAVEL_LIB_DIR}/imgui/imgui_demo.cpp"
    "${GRAVEL_LIB_DIR}/imgui/imgui_draw.cpp"
    "${GRAVEL_LIB_DIR}/imgui/imgui_internal.h"
    "${GRAVEL_LIB_DIR}/imgui/imgui_tables.cpp"
    "${GRAVEL_LIB_DIR}/imgui/imgui_widgets.cpp"
    "${GRAVEL_LIB_DIR}/imgui/imstb_rectpack.h"
    "${GRAVEL_LIB_DIR}/imgui/imstb_textedit.h"
    "${GRAVEL_LIB_DIR}/imgui/imstb_truetype.h"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_sdl2.cpp"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_sdl2.h"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_opengl3.h"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
    "${GRAVEL_LIB_DIR}/imgui/backends/imgui_impl_vulkan.h"
    )

add_library(imgui "${IMGUI_SOURCES}")

target_compile_definitions(imgui PRIVATE ${GRAVEL_COMPILE_DEFINITIONS})

target_include_directories(imgui
                           SYSTEM PUBLIC
                           "${GRAVEL_LIB_DIR}/imgui"
                           "${GRAVEL_LIB_DIR}/imgui/backends"
                           )

target_link_libraries(imgui
                      PUBLIC
                      SDL2::SDL2
                      Vulkan::Vulkan
                      )