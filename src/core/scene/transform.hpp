#pragma once

#include "common/type/math.hpp"

namespace glow {

/// Generic transform component for scene nodes.
struct Transform final {
  Vec3 position {0, 0, 0};  ///< Position translation.
  Vec3 rotation {0, 0, 0};  ///< Rotation along all axes.
  Vec3 scale {1, 1, 1};     ///< Scaling factor.

  /// Creates a 4x4 model matrix that encodes the translation, rotation, and scaling.
  [[nodiscard]] auto to_model_matrix() const -> Mat4;
};

/// Component used to indicate which transform members are meaningful.
struct TransformOptions final {
  bool use_position {true};
  bool use_rotation {true};
  bool use_scale {true};
};

}  // namespace glow