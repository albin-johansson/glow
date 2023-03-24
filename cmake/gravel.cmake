
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

  target_compile_definitions(${target}
                             PRIVATE
                             BOOST_ENABLE_ASSERT_DEBUG_HANDLER
                             BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
                             GLM_ENABLE_EXPERIMENTAL
                             IMGUI_DEFINE_MATH_OPERATORS
                             IMGUI_DISABLE_OBSOLETE_FUNCTIONS
                             NOMINMAX
                             SPDLOG_FMT_EXTERNAL
                             VK_NO_PROTOTYPES
                             WIN32_LEAN_AND_MEAN
                             )
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
