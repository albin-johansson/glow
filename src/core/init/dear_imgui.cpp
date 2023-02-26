#include "dear_imgui.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"

namespace gravel {

DearImGui::DearImGui(SDL_Window* window, const GraphicsApi api)
{
  IMGUI_CHECKVERSION();
  if (!ImGui::CreateContext()) {
    throw Error {"[GUI] Failed to create ImGui context"};
  }

  if (api == GraphicsApi::OpenGL) {
    mGL.emplace(window);
  }

  ImGui::StyleColorsDark();
}

DearImGui::~DearImGui()
{
  mGL.reset();
  ImGui::DestroyContext();
}

void DearImGui::reload_fonts()
{
  spdlog::debug("[GUI] Reloading fonts...");

  auto& io = ImGui::GetIO();
  const auto scale = io.DisplayFramebufferScale;

  io.Fonts->Clear();

  const float size = 13.0f;

  ImFontConfig config {};
  config.SizePixels = size * scale.x;
  io.Fonts->AddFontDefault(&config);

  // The global scale is 1 on most platforms, and 0.5 on macOS
  io.FontGlobalScale = 1.0f / scale.x;
  io.Fonts->Build();

  if (mGL) {
    mGL->recreate_font_textures();
  }

  ImGui::GetStyle().ScaleAllSizes(1.0f);
}

}  // namespace gravel
