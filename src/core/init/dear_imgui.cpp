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

  const ImVec4 transparent {0.00f, 0.00f, 0.00f, 0.00f};
  const ImVec4 dark_gray {0.02f, 0.02f, 0.02f, 1.00f};
  const ImVec4 background {0.01f, 0.01f, 0.01f, 1.00f};

  const ImVec4 primary_accent {0.00f, 0.20f, 0.25f, 1.00f};
  const ImVec4 primary_accent_hover {0.00f, 0.30f, 0.35f, 1.0f};
  const ImVec4 primary_accent_active {0.00f, 0.10f, 0.15f, 1.0f};

  const ImVec4 secondary_accent {1.00f, 0.20f, 0.90f, 1.0f};
  const ImVec4 secondary_accent_hover {1.00f, 0.25f, 0.95f, 1.0f};
  const ImVec4 secondary_accent_active {0.90f, 0.15f, 0.85f, 1.0f};

  style.Colors[ImGuiCol_Text] = ImVec4 {1.00f, 1.00f, 1.00f, 1.00f};
  style.Colors[ImGuiCol_TextDisabled] = ImVec4 {0.20f, 0.20f, 0.20f, 0.80f};
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4 {0.00f, 0.00f, 0.25f, 0.75f};

  style.Colors[ImGuiCol_WindowBg] = background;
  style.Colors[ImGuiCol_ChildBg] = transparent;
  style.Colors[ImGuiCol_PopupBg] = background;
  style.Colors[ImGuiCol_MenuBarBg] = dark_gray;

  style.Colors[ImGuiCol_TitleBg] = primary_accent;
  style.Colors[ImGuiCol_TitleBgCollapsed] = background;
  style.Colors[ImGuiCol_TitleBgActive] = primary_accent;

  style.Colors[ImGuiCol_FrameBg] = primary_accent;
  style.Colors[ImGuiCol_FrameBgHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_FrameBgActive] = primary_accent_active;

  style.Colors[ImGuiCol_Border] = primary_accent;
  style.Colors[ImGuiCol_BorderShadow] = transparent;

  style.Colors[ImGuiCol_CheckMark] = secondary_accent;

  style.Colors[ImGuiCol_ResizeGrip] = primary_accent;
  style.Colors[ImGuiCol_ResizeGripHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_ResizeGripActive] = primary_accent_active;

  style.Colors[ImGuiCol_Separator] = primary_accent;
  style.Colors[ImGuiCol_SeparatorHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_SeparatorActive] = primary_accent_active;

  style.Colors[ImGuiCol_Header] = primary_accent;
  style.Colors[ImGuiCol_HeaderHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_HeaderActive] = primary_accent_active;

  style.Colors[ImGuiCol_Tab] = primary_accent;
  style.Colors[ImGuiCol_TabHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_TabActive] = primary_accent_active;
  style.Colors[ImGuiCol_TabUnfocused] = primary_accent;
  style.Colors[ImGuiCol_TabUnfocusedActive] = primary_accent_active;

  style.Colors[ImGuiCol_Button] = primary_accent;
  style.Colors[ImGuiCol_ButtonHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_ButtonActive] = primary_accent_active;

  style.Colors[ImGuiCol_SliderGrab] = secondary_accent;
  style.Colors[ImGuiCol_SliderGrabActive] = secondary_accent_active;

  style.Colors[ImGuiCol_ScrollbarBg] = transparent;
  style.Colors[ImGuiCol_ScrollbarGrab] = primary_accent;
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = primary_accent_hover;
  style.Colors[ImGuiCol_ScrollbarGrabActive] = primary_accent_active;

  style.Colors[ImGuiCol_TableHeaderBg] = background;
  style.Colors[ImGuiCol_TableBorderStrong] = primary_accent;
  style.Colors[ImGuiCol_TableBorderLight] = transparent;

  style.Colors[ImGuiCol_TableRowBg] = transparent;
  style.Colors[ImGuiCol_TableRowBgAlt] = transparent;

  style.Colors[ImGuiCol_PlotLines] = secondary_accent;
  style.Colors[ImGuiCol_PlotLinesHovered] = secondary_accent_hover;

  style.Colors[ImGuiCol_PlotHistogram] = secondary_accent;
  style.Colors[ImGuiCol_PlotHistogramHovered] = secondary_accent_hover;

  style.Colors[ImGuiCol_DragDropTarget] = secondary_accent;

  style.Colors[ImGuiCol_NavHighlight] = secondary_accent;
  style.Colors[ImGuiCol_NavWindowingHighlight] = secondary_accent;
  style.Colors[ImGuiCol_NavWindowingDimBg] = transparent;
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
  io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf",
                               default_config.SizePixels,
                               &default_config);

  // The global scale is 1 on most platforms, and 0.5 on macOS
  io.FontGlobalScale = 1.0f / framebuffer_scale.x;

  ImFontConfig icon_config {};
  icon_config.MergeMode = true;
  icon_config.SizePixels = mFontSize * framebuffer_scale.x;
  icon_config.GlyphMinAdvanceX = icon_config.SizePixels;
  icon_config.GlyphMaxAdvanceX = icon_config.GlyphMinAdvanceX;
  icon_config.GlyphOffset = {0, 2};
  io.Fonts->AddFontFromFileTTF("assets/fonts/fa/" FONT_ICON_FILE_NAME_FAS,
                               icon_config.GlyphMinAdvanceX,
                               &icon_config,
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
