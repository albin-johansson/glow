#include "menu_bar.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>

#include "graphics/environment.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/rendering_options.hpp"
#include "scene/scene.hpp"
#include "ui/events.hpp"

namespace glow {
namespace {

void show_file_menu(const Scene&, Dispatcher& dispatcher)
{
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem(ICON_FA_ARROW_RIGHT_FROM_BRACKET " Exit")) {
      dispatcher.enqueue<QuitEvent>();
    }

    ImGui::EndMenu();
  }
}

void show_view_menu(const Scene&, Dispatcher& dispatcher)
{
  if (ImGui::BeginMenu("View")) {
    if (ImGui::MenuItem(ICON_FA_ROTATE " Reset UI")) {
      dispatcher.enqueue<RestoreLayoutEvent>();
    }

    if (ImGui::BeginMenu("Widgets")) {
      if (ImGui::MenuItem("Scene", nullptr, true)) {}

      if (ImGui::MenuItem("Viewport Overlay", nullptr, true)) {}

      ImGui::EndMenu();
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

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_FA_EYE " Toggle UI")) {
      // TODO dispatcher.enqueue<ToggleUIEvent>();
    }

    ImGui::EndMenu();
  }
}

void show_renderer_menu(const Scene& scene, Dispatcher& dispatcher)
{
  const auto& renderer_info = scene.get<RendererInfo>();
  const auto& rendering_options = scene.get<RenderingOptions>();

  bool show_renderer_info_popup = false;

  if (ImGui::BeginMenu("Renderer")) {
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

    show_renderer_info_popup = ImGui::MenuItem("Renderer Info...");

    ImGui::EndMenu();
  }

  static bool popup_open = false;

  if (show_renderer_info_popup) {
    ImGui::OpenPopup("Renderer Information");
    popup_open = true;
  }

  if (ImGui::BeginPopupModal("Renderer Information", &popup_open)) {
    ImGui::Text("API: %s", "OpenGL 4.1.0 core");
    ImGui::Text("Renderer: %s", renderer_info.renderer.c_str());
    ImGui::Text("Vendor: %s", renderer_info.vendor.c_str());
    ImGui::Text("Version: %s", renderer_info.version.c_str());

    ImGui::EndPopup();
  }
}

void show_environment_menu(const Scene& scene, Dispatcher& dispatcher)
{
  const auto& env_options = scene.get<EnvironmentOptions>();

  if (ImGui::BeginMenu("Environment")) {
    if (ImGui::MenuItem(ICON_FA_FILE_IMAGE " Load Environment Image...")) {
      // TODO
    }

    ImGui::Separator();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(ICON_FA_SUN " Brightness");
    ImGui::SameLine();

    float brightness = env_options.brightness;
    if (ImGui::SliderFloat("##Brightness", &brightness, 0.0f, 1.0f)) {
      dispatcher.enqueue<SetEnvironmentBrightnessEvent>(brightness);
    }

    ImGui::Separator();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(ICON_FA_CIRCLE_HALF_STROKE " Gamma Correction");
    ImGui::SameLine();

    bool use_gamma_correction = env_options.use_gamma_correction;
    if (ImGui::Checkbox("##GammaCorrection", &use_gamma_correction)) {
      dispatcher.enqueue<ToggleEnvironmentGammaCorrectionEvent>();
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(!env_options.use_gamma_correction);

    float gamma = env_options.gamma;
    if (ImGui::SliderFloat("##Gamma", &gamma, 1.0f, 4.0f)) {
      dispatcher.enqueue<SetEnvironmentGammaEvent>(gamma);
    }

    ImGui::EndDisabled();

    ImGui::EndMenu();
  }
}

void show_help_menu(const Scene&, Dispatcher& dispatcher)
{
  if (ImGui::BeginMenu("Help")) {
    ImGui::EndMenu();
  }
}

void show_debug_menu(const Scene&, Dispatcher& dispatcher)
{
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
}

}  // namespace

void show_menu_bar(const Scene& scene, Dispatcher& dispatcher)
{
  if (ImGui::BeginMainMenuBar()) {
    show_file_menu(scene, dispatcher);
    show_view_menu(scene, dispatcher);
    show_renderer_menu(scene, dispatcher);
    show_environment_menu(scene, dispatcher);
    show_help_menu(scene, dispatcher);
    show_debug_menu(scene, dispatcher);

    ImGui::EndMainMenuBar();
  }
}

}  // namespace glow
