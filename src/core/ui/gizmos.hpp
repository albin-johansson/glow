#pragma once

#include "common/predef.hpp"
#include "common/type/math.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_S(Transform);

enum class GizmoMode {
  Translate,
  Rotate,
  Scale
};

void show_model_control_gizmo(GizmoMode mode,
                              const Mat4& projection,
                              const Mat4& view,
                              Transform& transform);

}  // namespace gravel
