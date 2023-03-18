#include "dear_imgui.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"

namespace gravel {
namespace {

void init_style(ImGuiStyle& style)
{
  style.WindowPadding = ImVec2 {6, 6};
  style.FramePadding = ImVec2 {6, 3};
  style.ItemSpacing = ImVec2 {12, 4};
  style.ItemInnerSpacing = ImVec2 {4, 4};
  style.ScrollbarSize = 12;
  style.GrabMinSize = 10;

  style.WindowBorderSize = 0;
  style.ChildBorderSize = 0;
  style.PopupBorderSize = 0;
  style.FrameBorderSize = 0;
  style.TabBorderSize = 0;

  style.WindowRounding = 4;
  style.ChildRounding = 4;
  style.FrameRounding = 6;
  style.PopupRounding = 6;
  style.ScrollbarRounding = 4;
  style.GrabRounding = 4;
  style.TabRounding = 4;

  style.WindowTitleAlign = ImVec2 {0.5f, 0.5f};
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.SeparatorTextBorderSize = 2;
}

}  // namespace

DearImGui::DearImGui(SDL_Window* window, const GraphicsApi api)
{
  IMGUI_CHECKVERSION();
  if (!ImGui::CreateContext()) {
    throw Error {"[GUI] Failed to create ImGui context"};
  }

  auto& io = ImGui::GetIO();
  io.ConfigFlags |= static_cast<ImGuiConfigFlags>(ImGuiConfigFlags_DockingEnable);

  if (api == GraphicsApi::OpenGL) {
    mGL.emplace(window);
  }

  ImGui::StyleColorsDark();
  init_style(ImGui::GetStyle());
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
