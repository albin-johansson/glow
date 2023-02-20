#pragma once

#ifdef NDEBUG
#define GRAVEL_DEBUG_BUILD 0
#define GRAVEL_RELEASE_BUILD 1
#else
#define GRAVEL_DEBUG_BUILD 1
#define GRAVEL_RELEASE_BUILD 0
#endif  // NDEBUG

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