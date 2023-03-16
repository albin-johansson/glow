#pragma once

#include <boost/config.hpp>
#include <boost/predef/compiler.h>
#include <boost/predef/os.h>

#ifdef NDEBUG
#define GRAVEL_DEBUG_BUILD 0
#define GRAVEL_RELEASE_BUILD 1
#else
#define GRAVEL_DEBUG_BUILD 1
#define GRAVEL_RELEASE_BUILD 0
#endif  // NDEBUG

// Compiler detection
#define GRAVEL_COMPILER_MSVC BOOST_COMP_MSVC
#define GRAVEL_COMPILER_CLANG BOOST_COMP_CLANG
#define GRAVEL_COMPILER_GCC BOOST_COMP_GNUC

// OS detection
#define GRAVEL_OS_WINDOWS BOOST_OS_WINDOWS
#define GRAVEL_OS_LINUX BOOST_OS_LINUX
#define GRAVEL_OS_MACOS BOOST_OS_MACOS

// Attributes
#define GRAVEL_NOINLINE BOOST_NOINLINE

#if GRAVEL_OS_WINDOWS
#define GRAVEL_DEBUG_BREAK \
  do {                     \
  } while (false)
#elif __has_builtin(__builtin_debugtrap)
#define GRAVEL_DEBUG_BREAK __builtin_debugtrap()
#else
#define GRAVEL_DEBUG_BREAK \
  do {                     \
  } while (false)
#endif

// clang-format off
#define GRAVEL_DEFAULT_COPY(Class) \
  Class(const Class&) = default;   \
  auto operator=(const Class&) -> Class& = default

#define GRAVEL_DELETE_COPY(Class) \
  Class(const Class&) = delete;   \
  auto operator=(const Class&) -> Class& = delete

#define GRAVEL_DEFAULT_MOVE(Class) \
  Class(Class&&) noexcept = default;        \
  auto operator=(Class&&) noexcept -> Class& = default

#define GRAVEL_DELETE_MOVE(Class) \
  Class(Class&&) noexcept = delete;        \
  auto operator=(Class&&) noexcept -> Class& = delete
// clang-format on

#define GRAVEL_FORWARD_DECLARE_C(Class) class Class
#define GRAVEL_FORWARD_DECLARE_S(Class) struct Class

namespace gravel {

#if GRAVEL_DEBUG_BUILD
inline constexpr bool kDebugBuild = true;
inline constexpr bool kReleaseBuild = false;
#else
inline constexpr bool kDebugBuild = false;
inline constexpr bool kReleaseBuild = true;
#endif  // GRAVEL_DEBUG_BUILD

#ifdef __APPLE__
inline constexpr bool kOnApple = true;
#else
inline constexpr bool kOnApple = false;
#endif  // __APPLE__

}  // namespace gravel