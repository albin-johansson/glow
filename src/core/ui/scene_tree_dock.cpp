#include "scene_tree_dock.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>

#include "common/debug/assert.hpp"
#include "common/type/ecs.hpp"
#include "common/type/maybe.hpp"
#include "graphics/camera.hpp"
#include "graphics/opengl/model.hpp"
#include "scene/identifier.hpp"
#include "scene/node.hpp"
#include "scene/scene.hpp"
#include "scene/transform.hpp"
#include "ui/events.hpp"

namespace glow {
namespace {

[[nodiscard]] auto _show_drag_input(const char* label,
                                    const Vec3& vec,
                                    const float min = 0.0f,
                                    const float max = 0.0f,
                                    const ImGuiSliderFlags flags = 0) -> Maybe<Vec3>
{
  float buffer[3] {vec.x, vec.y, vec.z};
  if (ImGui::DragFloat3(label, buffer, 1.0f, min, max, "%.2f", flags)) {
    return Vec3 {buffer[0], buffer[1], buffer[2]};
  }
  else {
    return kNothing;
  }
}

void _show_transform_component(const Scene& scene,
                               const Transform& transform,
                               const Entity entity,
                               Dispatcher& dispatcher)
{
  ImGui::SeparatorText(ICON_FA_UP_DOWN_LEFT_RIGHT " Transform");

  const auto& transform_options = scene.get<TransformOptions>(entity);

  ImGui::BeginDisabled(!transform_options.use_position);
  if (const auto translation = _show_drag_input("Position", transform.position)) {
    dispatcher.enqueue<UpdateTransformTranslationEvent>(entity, *translation);
  }
  ImGui::EndDisabled();

  ImGui::BeginDisabled(!transform_options.use_rotation);
  if (const auto rotation = _show_drag_input("Rotation", transform.rotation)) {
    dispatcher.enqueue<UpdateTransformRotationEvent>(entity, *rotation);
  }
  ImGui::EndDisabled();

  ImGui::BeginDisabled(!transform_options.use_scale);
  if (const auto scale = _show_drag_input("Scale",
                                          transform.scale,
                                          0.001f,
                                          10'000,
                                          ImGuiSliderFlags_Logarithmic)) {
    dispatcher.enqueue<UpdateTransformScaleEvent>(entity, *scale);
  }
  ImGui::EndDisabled();
}

void _show_camera_component(const Scene& scene,
                            const Camera& camera,
                            const Entity entity,
                            Dispatcher& dispatcher)
{
  ImGui::SeparatorText(ICON_FA_CAMERA " Camera");
  ImGui::Text("Aspect ratio: %.2f", camera.aspect_ratio);

  bool active_camera {entity == scene.get<CameraContext>().active_camera};
  if (ImGui::Checkbox("Active", &active_camera)) {
    // TODO only allow enabling cameras, not disabling them
  }

  float fov = camera.fov;
  if (ImGui::SliderFloat("Field of view", &fov, 30.0f, 120.0f)) {
    dispatcher.enqueue<SetCameraFovEvent>(entity, fov);
  }

  ImGui::Spacing();

  float near_plane = camera.near_plane;
  if (ImGui::SliderFloat("Near plane", &near_plane, 0.01f, 1.0f)) {
    dispatcher.enqueue<SetCameraNearPlaneEvent>(entity, near_plane);
  }

  float far_plane = camera.far_plane;
  if (ImGui::SliderFloat("Far plane",
                         &far_plane,
                         near_plane,
                         near_plane + 1'000'000.0f)) {
    dispatcher.enqueue<SetCameraFarPlaneEvent>(entity, far_plane);
  }
}

void _show_scene_node_widget(const Scene& scene,
                             const Entity entity,
                             Dispatcher& dispatcher)
{
  GLOW_ASSERT(scene.get_registry().all_of<Node>(entity));

  ImGui::PushID(static_cast<int>(entity));

  const auto& node = scene.get<Node>(entity);
  const auto& identifier = scene.get<Identifier>(entity);

  if (ImGui::CollapsingHeader(identifier.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Indent();

    if (const auto* transform = scene.find<Transform>(entity)) {
      _show_transform_component(scene, *transform, entity, dispatcher);
    }

    if (const auto* camera = scene.find<Camera>(entity)) {
      _show_camera_component(scene, *camera, entity, dispatcher);
    }

    if (const auto* model = scene.find<gl::Model>(entity)) {
      ImGui::SeparatorText(ICON_FA_CUBES " Model (OpenGL)");
      ImGui::Text("Meshes: %zu", model->meshes.size());
    }

    ImGui::Spacing();

    for (const auto child_entity : node.children) {
      _show_scene_node_widget(scene, child_entity, dispatcher);
    }

    ImGui::Spacing();
    ImGui::Unindent();
  }

  ImGui::PopID();
}

}  // namespace

void show_scene_tree_dock(const Scene& scene, Dispatcher& dispatcher)
{
  if (ImGui::Begin("Scene")) {
    const auto& registry = scene.get_registry();
    for (auto [entity, node] : registry.view<Node>(entt::exclude<ChildNode>).each()) {
      _show_scene_node_widget(scene, entity, dispatcher);
    }
  }

  ImGui::End();
}

}  // namespace glow
