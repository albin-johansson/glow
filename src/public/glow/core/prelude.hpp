#pragma once

#include <cstddef>  // size_t, ptrdiff_t
#include <cstdint>  // uint{}_t, int{}_t, uintptr_t

#ifdef NDEBUG
  #define GLOW_DEBUG_BUILD 0
#else
  #define GLOW_DEBUG_BUILD 1
#endif  // NDEBUG

// TODO validate
#if defined(_MSC_VER)
  #define GLOW_COMPILER_MSVC 1
  #define GLOW_COMPILER_CLANG 0
  #define GLOW_COMPILER_GCC 0
#elif defined(__clang__)
  #define GLOW_COMPILER_MSVC 0
  #define GLOW_COMPILER_CLANG 1
  #define GLOW_COMPILER_GCC 0
#elif defined(__GNUC__)
  #define GLOW_COMPILER_MSVC 0
  #define GLOW_COMPILER_CLANG 0
  #define GLOW_COMPILER_GCC 1
#else
  #define GLOW_COMPILER_MSVC 0
  #define GLOW_COMPILER_CLANG 0
  #define GLOW_COMPILER_GCC 0
#endif  // defined(_MSC_VER)

#if GLOW_COMPILER_MSVC
  #define GLOW_NOINLINE __declspec(noinline)
#elif GLOW_COMPILER_CLANG
  #define GLOW_NOINLINE [[clang::noinline]]
#elif GLOW_COMPILER_GCC
  #define GLOW_NOINLINE __attribute__((noinline))
#else
  #define GLOW_NOINLINE
#endif  // GLOW_COMPILER_MSVC

#if GLOW_COMPILER_MSVC
  #define GLOW_EXPORT __declspec(dllexport)
  #define GLOW_IMPORT __declspec(dllimport)
#elif GLOW_COMPILER_CLANG
  #define GLOW_EXPORT __attribute__((__visibility__("default")))
  #define GLOW_IMPORT
#elif GLOW_COMPILER_GCC
  #define GLOW_EXPORT __attribute__((dllexport))
  #define GLOW_IMPORT __attribute__((dllimport))
#else
  #define GLOW_EXPORT
  #define GLOW_IMPORT
#endif  // GLOW_COMPILER_MSVC

#ifdef GLOW_BUILDING_CORE
  #define GLOW_CORE_API GLOW_EXPORT
#else
  #define GLOW_CORE_API GLOW_IMPORT
#endif  // GLOW_BUILDING_CORE

#define GLOW_CANNOT_COPY(Name)      \
  Name(const Name& other) = delete; \
  Name& operator=(const Name& other) = delete

#define GLOW_DEFAULT_COPY(Name)      \
  Name(const Name& other) = default; \
  Name& operator=(const Name& other) = default

#define GLOW_CANNOT_MOVE(Name)          \
  Name(Name&& other) noexcept = delete; \
  Name& operator=(Name&& other) noexcept = delete

#define GLOW_DEFAULT_MOVE(Name)          \
  Name(Name&& other) noexcept = default; \
  Name& operator=(Name&& other) noexcept = default

namespace glow {

using usize = std::size_t;
using ssize = std::ptrdiff_t;
using uintptr = std::uintptr_t;

using uchar = unsigned char;
using uint = unsigned int;
using ulonglong = unsigned long long int;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

static_assert(sizeof(float32) == 4);
static_assert(sizeof(float64) == 8);

}  // namespace glow