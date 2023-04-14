#pragma once

#include "common/type/string.hpp"

namespace glow {

/// Basic identifier component.
struct Identifier final {
  String name;
};

}  // namespace glow