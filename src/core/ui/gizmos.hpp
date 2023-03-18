#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"
#include "common/type/math.hpp"
#include "common/type/ecs.hpp"

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
                              Entity entity,
                              const Transform& transform,
                              Dispatcher& dispatcher);

}  // namespace gravel
