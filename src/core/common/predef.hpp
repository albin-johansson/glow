#pragma once

#ifdef NDEBUG
#define GRAVEL_DEBUG_BUILD 0
#define GRAVEL_RELEASE_BUILD 1
#else
#define GRAVEL_DEBUG_BUILD 1
#define GRAVEL_RELEASE_BUILD 0
#endif  // NDEBUG

#ifdef WIN32
#define GRAVEL_DEBUG_BREAK \
  do {                     \
  } while (false)
#else
#if __has_builtin(__builtin_debugtrap)
#define GRAVEL_DEBUG_BREAK __builtin_debugtrap()
#else
#define GRAVEL_DEBUG_BREAK \
  do {                     \
  } while (false)
#endif  // __has_builtin(__builtin_debugtrap)
#endif  // WIN32

// clang-format off
#define GRAVEL_DELETE_COPY(Class) \
  Class(const Class&) = delete;   \
  auto operator=(const Class&) -> Class& = delete

#define GRAVEL_DELETE_MOVE(Class) \
  Class(Class&&) noexcept = delete;        \
  auto operator=(Class&&) noexcept -> Class& = delete
// clang-format on

#define GRAVEL_FORWARD_DECLARE(Class) class Class

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