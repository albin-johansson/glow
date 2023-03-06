#pragma once

#include "common/type/math.hpp"

namespace gravel::comp {

struct Transform final {
  Vec3 translation {0, 0, 0};
  Vec3 rotation {0, 0, 0};
  Vec3 scale {1, 1, 1};
};

}  // namespace gravel::comp