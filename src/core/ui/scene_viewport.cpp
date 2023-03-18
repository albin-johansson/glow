#include "scene_viewport.hpp"

#include <imgui.h>
#include <imgui_internal.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "engine/backend.hpp"
#include "scene/scene.hpp"
#include "ui/camera_options.hpp"
#include "ui/events.hpp"

namespace gravel {
namespace {

void show_generic_viewport_context_menu(Backend& backend)
{
  if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
    ImGui::SeparatorText("Transform Mode");

    if (ImGui::MenuItem("Translate",
                        nullptr,
                        backend.get_gizmo_mode() == GizmoMode::Translate)) {
      backend.set_gizmo_mode(GizmoMode::Translate);
    }

    if (ImGui::MenuItem("Rotate",
                        nullptr,
                        backend.get_gizmo_mode() == GizmoMode::Rotate)) {
      backend.set_gizmo_mode(GizmoMode::Rotate);
    }

    if (ImGui::MenuItem("Scale", nullptr, backend.get_gizmo_mode() == GizmoMode::Scale)) {
      backend.set_gizmo_mode(GizmoMode::Scale);
    }

    ImGui::EndPopup();
  }
}

}  // namespace

void show_scene_viewport(const Scene& scene, Backend& backend, Dispatcher& dispatcher)
{
  ImGuiWindowClass wc {};
  wc.DockNodeFlagsOverrideSet =
      ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
  ImGui::SetNextWindowClass(&wc);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

  if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    const auto& io = ImGui::GetIO();

    const auto window_pos = ImGui::GetWindowPos();
    const auto window_size = ImGui::GetWindowSize();
    ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);

    if (!ImGuizmo::IsUsing() && ImGui::IsItemActive() && ImGui::IsWindowHovered()) {
      const auto mouse_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.05f);

      const auto& registry = scene.get_registry();
      const auto& camera_options = registry.ctx().get<CameraOptions>();

      const auto dt = io.DeltaTime;
      const auto yaw = camera_options.sensitivity * -mouse_delta.x * dt;
      const auto pitch = camera_options.sensitivity * -mouse_delta.y * dt;
      dispatcher.enqueue<RotateActiveCameraEvent>(yaw, pitch);

      ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    const auto framebuffer_size = window_size * io.DisplayFramebufferScale;
    backend.render_scene(scene,
                         Vec2 {framebuffer_size.x, framebuffer_size.y},
                         dispatcher);

    ImGui::Image(backend.get_primary_framebuffer_handle(),
                 ImGui::GetContentRegionAvail(),
                 ImVec2 {0, 1},
                 ImVec2 {1, 0});
  }

  ImGui::PopStyleVar();  // ImGuiStyleVar_WindowPadding

  show_generic_viewport_context_menu(backend);

  ImGui::End();
}

}  // namespace gravel
