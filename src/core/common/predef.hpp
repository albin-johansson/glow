#pragma once

#include <boost/config.hpp>
#include <boost/predef/compiler.h>
#include <boost/predef/os.h>
#include <boost/version.hpp>

#include "common/primitives.hpp"

#define GLOW_VERSION_MAJOR 0
#define GLOW_VERSION_MINOR 1
#define GLOW_VERSION_PATCH 0

#define GLOW_VERSION \
  BOOST_VERSION_NUMBER(GLOW_VERSION_MAJOR, GLOW_VERSION_MINOR, GLOW_VERSION_PATCH)

// clang-format off
#define GLOW_VERSION_STRING           \
  BOOST_STRINGIZE(GLOW_VERSION_MAJOR) "." \
  BOOST_STRINGIZE(GLOW_VERSION_MINOR) "." \
  BOOST_STRINGIZE(GLOW_VERSION_PATCH)
// clang-format on

#ifdef NDEBUG
#define GLOW_DEBUG_BUILD 0
#define GLOW_RELEASE_BUILD 1
#else
#define GLOW_DEBUG_BUILD 1
#define GLOW_RELEASE_BUILD 0
#endif  // NDEBUG

// Compiler detection
#define GLOW_COMPILER_MSVC BOOST_COMP_MSVC
#define GLOW_COMPILER_CLANG BOOST_COMP_CLANG
#define GLOW_COMPILER_GCC BOOST_COMP_GNUC

// OS detection
#define GLOW_OS_WINDOWS BOOST_OS_WINDOWS
#define GLOW_OS_LINUX BOOST_OS_LINUX
#define GLOW_OS_MACOS BOOST_OS_MACOS

// Attributes
#define GLOW_NOINLINE BOOST_NOINLINE

#if GLOW_OS_WINDOWS
#define GLOW_DEBUG_BREAK \
  do {                   \
  } while (false)
#elif __has_builtin(__builtin_debugtrap)
#define GLOW_DEBUG_BREAK __builtin_debugtrap()
#else
#define GLOW_DEBUG_BREAK \
  do {                   \
  } while (false)
#endif

// clang-format off
#define GLOW_DEFAULT_COPY(Class) \
  Class(const Class&) = default;   \
  auto operator=(const Class&) -> Class& = default

#define GLOW_DELETE_COPY(Class) \
  Class(const Class&) = delete;   \
  auto operator=(const Class&) -> Class& = delete

#define GLOW_DEFAULT_MOVE(Class) \
  Class(Class&&) noexcept = default;        \
  auto operator=(Class&&) noexcept -> Class& = default

#define GLOW_DELETE_MOVE(Class) \
  Class(Class&&) noexcept = delete;        \
  auto operator=(Class&&) noexcept -> Class& = delete
// clang-format on

// Convenience macro used to work around issues with move-only components.
#define GLOW_MOVE_ONLY_COMPONENT(Class) \
  Class() = default;                    \
  ~Class() = default;                   \
  GLOW_DEFAULT_MOVE(Class);             \
  GLOW_DELETE_COPY(Class)

#define GLOW_FORWARD_DECLARE_C(Class) class Class
#define GLOW_FORWARD_DECLARE_S(Class) struct Class

namespace glow {

inline constexpr uint32 kVersion = GLOW_VERSION;

inline constexpr bool kDebugBuild = GLOW_DEBUG_BUILD;
inline constexpr bool kReleaseBuild = GLOW_RELEASE_BUILD;

inline constexpr bool kIsMacOS = GLOW_OS_MACOS;
inline constexpr bool kIsWindows = GLOW_OS_WINDOWS;
inline constexpr bool kIsLinux = GLOW_OS_LINUX;

}  // namespace glow