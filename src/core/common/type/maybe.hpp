#pragma once

#include <optional>  // optional, nullopt_t, nullopt

namespace glow {

template <typename T>
using Maybe = std::optional<T>;

inline constexpr std::nullopt_t kNothing = std::nullopt;

}  // namespace glow