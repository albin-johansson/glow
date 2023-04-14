#include "dear_imgui_vulkan.hpp"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "common/debug/error.hpp"
#include "graphics/vulkan/cmd/command_buffer.hpp"

namespace gravel {

DearImGuiVulkan::DearImGuiVulkan(SDL_Window* window)
{
  if (!ImGui_ImplSDL2_InitForVulkan(window)) {
    throw Error {"[GUI] Could not initialize DearImGui SDL2 backend"};
  }
}

DearImGuiVulkan::~DearImGuiVulkan()
{
  ImGui_ImplSDL2_Shutdown();
}

void DearImGuiVulkan::recreate_font_textures()
{
  vk::execute_immediately([](VkCommandBuffer cmd_buffer) {
    ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
  });

  ImGui_ImplVulkan_DestroyFontUploadObjects();
}

}  // namespace gravel