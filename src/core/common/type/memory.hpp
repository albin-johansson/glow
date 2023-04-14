#pragma once

#include <memory>  // unique_ptr, shared_ptr, weak_ptr, default_delete

namespace glow {

template <typename T, typename Deleter = std::default_delete<T>>
using Unique = std::unique_ptr<T, Deleter>;

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T>
using Weak = std::weak_ptr<T>;

}  // namespace glow
