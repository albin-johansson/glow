#include "menu_bar.hpp"

#include <imgui.h>

#include "graphics/rendering_options.hpp"
#include "scene/scene.hpp"
#include "ui/events.hpp"

namespace gravel {

void show_menu_bar(const Scene& scene, Dispatcher& dispatcher)
{
  const auto& registry = scene.get_registry();
  const auto& rendering_options = registry.ctx().get<RenderingOptions>();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {

      if (ImGui::MenuItem("Exit")) {
        dispatcher.enqueue<QuitEvent>();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Reset UI")) {
        dispatcher.enqueue<RestoreLayoutEvent>();
      }

      ImGui::Separator();

      if (ImGui::MenuItem("VSync",
                          nullptr,
                          rendering_options.test(RenderingOption::VSync))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::VSync);
      }

      if (ImGui::MenuItem("Depth Test",
                          nullptr,
                          rendering_options.test(RenderingOption::DepthTest))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::DepthTest);
      }

      if (ImGui::MenuItem("Face Culling",
                          nullptr,
                          rendering_options.test(RenderingOption::FaceCulling))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::FaceCulling);
      }

      if (ImGui::MenuItem("Wireframe Rendering",
                          nullptr,
                          rendering_options.test(RenderingOption::Wireframe))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::Wireframe);
      }

      if (ImGui::MenuItem("Blending",
                          nullptr,
                          rendering_options.test(RenderingOption::Blending))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::Blending);
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Debug")) {
      if (ImGui::MenuItem("Open Style Editor...")) {
        dispatcher.enqueue<ShowStyleEditorEvent>();
      }

      if (ImGui::MenuItem("Open Demo Window...")) {
        dispatcher.enqueue<ShowDemoWindowEvent>();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

}  // namespace gravel
