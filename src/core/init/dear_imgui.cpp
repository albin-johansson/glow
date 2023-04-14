#include "dear_imgui.hpp"

#include <algorithm>  // min, max

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"

namespace gravel {
namespace {

inline constexpr ImWchar kFontIconRange[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

void init_style(ImGuiStyle& style)
{
  style.WindowPadding = ImVec2 {6, 6};
  style.FramePadding = ImVec2 {6, 3};
  style.ItemSpacing = ImVec2 {12, 6};
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
  // io.ConfigFlags |= static_cast<ImGuiConfigFlags>(ImGuiConfigFlags_DockingEnable);

  if (api == GraphicsApi::OpenGL) {
    mGL.emplace(window);
  }

  if (api == GraphicsApi::Vulkan) {
    mVK.emplace(window);
  }

  ImGui::StyleColorsDark();
  init_style(ImGui::GetStyle());
}

DearImGui::~DearImGui()
{
  mGL.reset();
  mVK.reset();
  ImGui::DestroyContext();
}

void DearImGui::reload_fonts()
{
  spdlog::debug("[GUI] Reloading fonts...");

  auto& io = ImGui::GetIO();
  const auto& framebuffer_scale = io.DisplayFramebufferScale;

  io.Fonts->Clear();

  ImFontConfig default_config {};
  default_config.SizePixels = mFontSize * framebuffer_scale.x;
  io.Fonts->AddFontDefault(&default_config);

  // The global scale is 1 on most platforms, and 0.5 on macOS
  io.FontGlobalScale = 1.0f / framebuffer_scale.x;

  ImFontConfig fa_config {};
  fa_config.MergeMode = true;
  fa_config.SizePixels = mFontSize * framebuffer_scale.x;
  fa_config.GlyphMinAdvanceX = fa_config.SizePixels;
  fa_config.GlyphMaxAdvanceX = fa_config.GlyphMinAdvanceX;
  fa_config.GlyphOffset = {0, 2};
  io.Fonts->AddFontFromFileTTF("assets/fonts/fa/" FONT_ICON_FILE_NAME_FAS,
                               fa_config.GlyphMinAdvanceX,
                               &fa_config,
                               kFontIconRange);

  io.Fonts->Build();

  if (mGL) {
    mGL->recreate_font_textures();
  }

  if (mVK) {
    mVK->recreate_font_textures();
  }

  ImGui::GetStyle().ScaleAllSizes(1.0f);
}

void DearImGui::increase_font_size()
{
  mFontSize += 2;
  mFontSize = std::min(kMaxFontSize, mFontSize);

  reload_fonts();
}

void DearImGui::decrease_font_size()
{
  mFontSize -= 2;
  mFontSize = std::max(kMinFontSize, mFontSize);

  reload_fonts();
}

void DearImGui::reset_font_size()
{
  mFontSize = kDefFontSize;
  reload_fonts();
}

}  // namespace gravel
