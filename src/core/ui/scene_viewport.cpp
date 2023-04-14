#include "scene_viewport.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <imgui_internal.h>

// This comment prevents moving the following include before <imgui.h>
#include <ImGuizmo.h>

#include "engine/backend.hpp"
#include "scene/scene.hpp"
#include "ui/camera_options.hpp"
#include "ui/events.hpp"
#include "ui/gizmos.hpp"

namespace gravel {
namespace {

void show_generic_viewport_context_menu(const Scene& scene, Dispatcher& dispatcher)
{
  if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
    ImGui::SeparatorText(ICON_FA_UP_DOWN_LEFT_RIGHT " Transform Options");

    const auto& gizmos_options = scene.get<GizmosOptions>();
    const auto& camera_options = scene.get<CameraOptions>();

    if (ImGui::MenuItem(ICON_FA_LOCATION_DOT " Translate",
                        nullptr,
                        gizmos_options.operation == GizmoOperation::Translate)) {
      dispatcher.enqueue<SetGizmoOperationEvent>(GizmoOperation::Translate);
    }

    if (ImGui::MenuItem(ICON_FA_ROTATE " Rotate",
                        nullptr,
                        gizmos_options.operation == GizmoOperation::Rotate)) {
      dispatcher.enqueue<SetGizmoOperationEvent>(GizmoOperation::Rotate);
    }

    if (ImGui::MenuItem(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER " Scale",
                        nullptr,
                        gizmos_options.operation == GizmoOperation::Scale)) {
      dispatcher.enqueue<SetGizmoOperationEvent>(GizmoOperation::Scale);
    }

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(ICON_FA_GLOBE " Mode");
    ImGui::SameLine();

    int mode_index = (gizmos_options.mode == GizmoMode::Local) ? 0 : 1;
    if (ImGui::Combo("##Mode", &mode_index, "Local\0World\0\0")) {
      dispatcher.enqueue<SetGizmoModeEvent>((mode_index == 0) ? GizmoMode::Local
                                                              : GizmoMode::World);
    }

    ImGui::SeparatorText(ICON_FA_CAMERA " Camera Options");

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(ICON_FA_PERSON_RUNNING);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);

    float speed = camera_options.speed;
    if (ImGui::SliderFloat("##Speed",
                           &speed,
                           0,
                           10'000,
                           "Speed: %.1f",
                           ImGuiSliderFlags_Logarithmic)) {
      dispatcher.enqueue<SetCameraSpeedEvent>(speed);
    }

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(ICON_FA_COMPUTER_MOUSE);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);

    float sensitivity = camera_options.sensitivity;
    if (ImGui::SliderFloat("##Sensitivity",
                           &sensitivity,
                           0.1f,
                           2.0f,
                           "Sensitivity: %.1f")) {
      dispatcher.enqueue<SetCameraSensitivityEvent>(sensitivity);
    }

    ImGui::EndPopup();
  }
}

void show_performance_overlay()
{
  const auto pos = ImGui::GetWindowPos();
  const auto size = ImGui::GetWindowSize();

  const auto padding = ImGui::GetStyle().FramePadding;
  const ImVec2 next_pos {pos.x + size.x - padding.x, pos.y + padding.y};
  const ImVec2 next_pivot {1.0f, 0.0f};

  ImGui::SetNextWindowPos(next_pos, ImGuiCond_Always, next_pivot);
  ImGui::SetNextWindowBgAlpha(0.5f);

  const auto window_flags = ImGuiWindowFlags_NoDecoration |
                            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav |
                            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoFocusOnAppearing;

  if (ImGui::Begin("##PerformanceOverlay", nullptr, window_flags)) {
    const auto& io = ImGui::GetIO();

    ImGui::SeparatorText("Performance");
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame: %.2f ms", io.DeltaTime * 1'000.0f);

    ImGui::SeparatorText("UI");
    ImGui::Text("Vertices: %i", io.MetricsRenderVertices);
    ImGui::Text("Indices: %i", io.MetricsRenderIndices);
  }
  ImGui::End();
}

}  // namespace

void show_scene_viewport(const Scene& scene, Backend& backend, Dispatcher& dispatcher)
{
//  ImGuiWindowClass wc {};
//  wc.DockNodeFlagsOverrideSet =
//      ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;
//  ImGui::SetNextWindowClass(&wc);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

  if (ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    const auto& io = ImGui::GetIO();

    const auto window_pos = ImGui::GetWindowPos();
    const auto window_size = ImGui::GetWindowSize();
    ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);

    if (!ImGuizmo::IsUsing() && ImGui::IsItemActive() && ImGui::IsWindowHovered()) {
      const auto mouse_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.05f);

      const auto& camera_options = scene.get<CameraOptions>();
      const auto yaw = camera_options.sensitivity * -mouse_delta.x * io.DeltaTime;
      const auto pitch = camera_options.sensitivity * -mouse_delta.y * io.DeltaTime;
      dispatcher.enqueue<RotateActiveCameraEvent>(yaw, pitch);

      ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    const auto framebuffer_size = window_size * io.DisplayFramebufferScale;
    backend.render_scene(scene,
                         Vec2 {framebuffer_size.x, framebuffer_size.y},
                         dispatcher);

    if (auto* fbo = backend.get_primary_framebuffer_handle()) {
      ImGui::Image(fbo, ImGui::GetContentRegionAvail(), ImVec2 {0, 1}, ImVec2 {1, 0});
    }
  }

  ImGui::PopStyleVar();  // ImGuiStyleVar_WindowPadding

  show_generic_viewport_context_menu(scene, dispatcher);
  show_performance_overlay();

  ImGui::End();
}

}  // namespace gravel
