#pragma once

#include <utility>  // to_underlying

#define GLOW_FWD_FLAG_ENUM(Name, Type) \
  enum class Name : Type;              \
  using Name##Flags = Name

/** Generates bitwise operator overloads for enum classes. */
#define GLOW_FLAG_ENUM(Enum)                                                          \
  [[nodiscard]] constexpr auto operator~(const Enum a) noexcept -> Enum               \
  {                                                                                   \
    return Enum {~std::to_underlying(a)};                                             \
  }                                                                                   \
                                                                                      \
  [[nodiscard]] constexpr auto operator&(const Enum a, const Enum b) noexcept -> Enum \
  {                                                                                   \
    return Enum {std::to_underlying(a) & std::to_underlying(b)};                      \
  }                                                                                   \
                                                                                      \
  [[nodiscard]] constexpr auto operator|(const Enum a, const Enum b) noexcept -> Enum \
  {                                                                                   \
    return Enum {std::to_underlying(a) | std::to_underlying(b)};                      \
  }                                                                                   \
                                                                                      \
  [[nodiscard]] constexpr auto operator^(const Enum a, const Enum b) noexcept -> Enum \
  {                                                                                   \
    return Enum {std::to_underlying(a) ^ std::to_underlying(b)};                      \
  }                                                                                   \
                                                                                      \
  constexpr auto operator&=(Enum& a, const Enum b) noexcept -> Enum&                  \
  {                                                                                   \
    a = a & b;                                                                        \
    return a;                                                                         \
  }                                                                                   \
                                                                                      \
  constexpr auto operator|=(Enum& a, const Enum b) noexcept -> Enum&                  \
  {                                                                                   \
    a = a | b;                                                                        \
    return a;                                                                         \
  }                                                                                   \
                                                                                      \
  constexpr auto operator^=(Enum& a, const Enum b) noexcept -> Enum&                  \
  {                                                                                   \
    a = a ^ b;                                                                        \
    return a;                                                                         \
  }
