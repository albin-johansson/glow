#include "menu_bar.hpp"

#include <IconsFontAwesome6.h>
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
      if (ImGui::MenuItem(ICON_FA_FILE_IMPORT " Load Model...")) {
        dispatcher.enqueue<QuitEvent>();
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_ARROW_RIGHT_FROM_BRACKET " Exit")) {
        dispatcher.enqueue<QuitEvent>();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem(ICON_FA_ROTATE " Reset UI")) {
        dispatcher.enqueue<RestoreLayoutEvent>();
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_GAUGE_HIGH " VSync",
                          nullptr,
                          rendering_options.test(RenderingOption::VSync))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::VSync);
      }

      if (ImGui::MenuItem(ICON_FA_LAYER_GROUP " Depth Test",
                          nullptr,
                          rendering_options.test(RenderingOption::DepthTest))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::DepthTest);
      }

      if (ImGui::MenuItem(ICON_FA_EYE_SLASH " Face Culling",
                          nullptr,
                          rendering_options.test(RenderingOption::FaceCulling))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::FaceCulling);
      }

      if (ImGui::MenuItem(ICON_FA_CIRCLE_NODES " Wireframe Rendering",
                          nullptr,
                          rendering_options.test(RenderingOption::Wireframe))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::Wireframe);
      }

      if (ImGui::MenuItem(ICON_FA_DROPLET " Blending",
                          nullptr,
                          rendering_options.test(RenderingOption::Blending))) {
        dispatcher.enqueue<ToggleRenderingOptionEvent>(RenderingOption::Blending);
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_MAGNIFYING_GLASS_PLUS " Increase Font Size")) {
        dispatcher.enqueue<IncreaseFontSizeEvent>();
      }

      if (ImGui::MenuItem(ICON_FA_MAGNIFYING_GLASS_MINUS " Decrease Font Size")) {
        dispatcher.enqueue<DecreaseFontSizeEvent>();
      }

      if (ImGui::MenuItem(ICON_FA_FONT " Reset Font Size")) {
        dispatcher.enqueue<ResetFontSizeEvent>();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Debug")) {
      if (ImGui::MenuItem(ICON_FA_PALETTE " Open Style Editor...")) {
        dispatcher.enqueue<ShowStyleEditorEvent>();
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_CIRCLE_INFO " Open Demo Window...")) {
        dispatcher.enqueue<ShowDemoWindowEvent>();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

}  // namespace gravel
