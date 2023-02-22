#pragma once

#include "common/type/math.hpp"

namespace gravel {

struct Vertex final {
  Vec3 position {};
  Vec3 normal {};
  Vec2 tex_coords {};
};

}  // namespace gravel