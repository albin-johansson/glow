#pragma once

#include "common/type/math.hpp"

namespace gravel {

/// Generic transform component for scene nodes.
struct Transform final {
  Vec3 translation {0, 0, 0};  ///< Position translation.
  Vec3 rotation {0, 0, 0};     ///< Rotation along all axes, in degrees.
  Vec3 scale {1, 1, 1};        ///< Model scaling factor.

  /// Creates a 4x4 model matrix that encodes the translation, rotation, and scaling.
  [[nodiscard]] auto to_model_matrix() const -> Mat4;
};

}  // namespace gravel