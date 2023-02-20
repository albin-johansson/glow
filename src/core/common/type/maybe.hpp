#pragma once

#include <optional>  // optional, nullopt_t, nullopt

namespace gravel {

template <typename T>
using Maybe = std::optional<T>;

inline constexpr std::nullopt_t kNothing = std::nullopt;

}  // namespace gravel