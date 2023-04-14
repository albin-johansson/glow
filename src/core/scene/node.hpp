#pragma once

#include "common/type/ecs.hpp"
#include "common/type/vector.hpp"

namespace glow {

/// Scene node component.
struct Node final {
  Entity parent {kNullEntity};  ///< The parent node entity, if any.
  Vector<Entity> children;      ///< All child node entities.
};

/// Tag component for nodes that are descendants of another node.
struct ChildNode final {};

}  // namespace glow