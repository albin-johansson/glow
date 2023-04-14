#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"
#include "common/type/math.hpp"
#include "common/type/ecs.hpp"

namespace glow {

GRAVEL_FORWARD_DECLARE_S(Transform);

enum class GizmoOperation {
  Translate,
  Rotate,
  Scale
};

enum class GizmoMode {
  Local,
  World
};

/// Context component for general gizmo settings.
struct GizmosOptions final {
  GizmoOperation operation {GizmoOperation::Translate};
  GizmoMode mode {GizmoMode::Local};
};

void show_model_control_gizmo(const GizmosOptions& options,
                              const Mat4& projection,
                              const Mat4& view,
                              Entity entity,
                              const Transform& transform,
                              Dispatcher& dispatcher);

}  // namespace glow
