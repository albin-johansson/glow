#include "gizmos.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "common/debug/error.hpp"
#include "scene/transform.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto convert_gizmo_mode(const GizmoMode mode) -> ImGuizmo::OPERATION
{
  switch (mode) {
    case GizmoMode::Translate:
      return ImGuizmo::TRANSLATE;

    case GizmoMode::Rotate:
      return ImGuizmo::ROTATE;

    case GizmoMode::Scale:
      return ImGuizmo::SCALE;

    default:
      throw Error {"Invalid gizmo mode"};
  }
}

}  // namespace

void show_model_control_gizmo(const GizmoMode mode,
                              const Mat4& projection,
                              const Mat4& view,
                              Transform& transform)
{
  float transform_matrix[16] {};
  ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transform.translation),
                                          glm::value_ptr(transform.rotation),
                                          glm::value_ptr(transform.scale),
                                          transform_matrix);

  if (ImGuizmo::Manipulate(glm::value_ptr(view),
                           glm::value_ptr(projection),
                           convert_gizmo_mode(mode),
                           ImGuizmo::WORLD,
                           transform_matrix)) {
    float translation[3] {};
    float rotation[3] {};
    float scale[3] {};
    ImGuizmo::DecomposeMatrixToComponents(transform_matrix, translation, rotation, scale);

    auto to_vec3 = [](const float array[3]) {
      return Vec3 {array[0], array[1], array[2]};
    };

    transform.translation = to_vec3(translation);
    transform.rotation = to_vec3(rotation);
    transform.scale = to_vec3(scale);
  }
}

}  // namespace gravel