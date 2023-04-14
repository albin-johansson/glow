#include "gizmos.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/error.hpp"
#include "scene/transform.hpp"
#include "ui/events.hpp"

namespace glow {
namespace {

[[nodiscard]] auto convert_gizmo_operation(const GizmoOperation operation)
    -> ImGuizmo::OPERATION
{
  switch (operation) {
    case GizmoOperation::Translate:
      return ImGuizmo::TRANSLATE;

    case GizmoOperation::Rotate:
      return ImGuizmo::ROTATE;

    case GizmoOperation::Scale:
      return ImGuizmo::SCALE;

    default:
      throw Error {"Invalid gizmo operation"};
  }
}

[[nodiscard]] auto convert_gizmo_mode(const GizmoMode mode) -> ImGuizmo::MODE
{
  switch (mode) {
    case GizmoMode::Local:
      return ImGuizmo::LOCAL;

    case GizmoMode::World:
      return ImGuizmo::WORLD;

    default:
      throw Error {"Invalid gizmo mode"};
  }
}

}  // namespace

void show_model_control_gizmo(const GizmosOptions& options,
                              const Mat4& projection,
                              const Mat4& view,
                              const Entity entity,
                              const Transform& transform,
                              Dispatcher& dispatcher)
{
  float transform_matrix[16] {};
  ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transform.position),
                                          glm::value_ptr(transform.rotation),
                                          glm::value_ptr(transform.scale),
                                          transform_matrix);

  if (ImGuizmo::Manipulate(glm::value_ptr(view),
                           glm::value_ptr(projection),
                           convert_gizmo_operation(options.operation),
                           convert_gizmo_mode(options.mode),
                           transform_matrix)) {
    float translation[3] {};
    float rotation[3] {};
    float scale[3] {};
    ImGuizmo::DecomposeMatrixToComponents(transform_matrix, translation, rotation, scale);

    auto to_vec3 = [](const float array[3]) {
      return Vec3 {array[0], array[1], array[2]};
    };

    dispatcher.enqueue<UpdateTransformEvent>(entity,
                                             to_vec3(translation),
                                             to_vec3(rotation),
                                             to_vec3(scale));
  }
}

}  // namespace glow