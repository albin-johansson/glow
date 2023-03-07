#pragma once

#include "common/type/ecs.hpp"
#include "common/type/vector.hpp"

namespace gravel::comp {

struct Node final {
  Entity parent {kNullEntity};
  Vector<Entity> children;
};

/// Tag for nodes that are descendants of some other node (other than the root node).
struct ChildNode final {};

}  // namespace gravel::comp