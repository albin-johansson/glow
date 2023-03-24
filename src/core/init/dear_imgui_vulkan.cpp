#include "dear_imgui_vulkan.hpp"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "common/debug/error.hpp"

namespace gravel {

DearImGuiVulkan::DearImGuiVulkan(SDL_Window* window)
{
  if (!ImGui_ImplSDL2_InitForVulkan(window)) {
    throw Error {"[GUI] Could not initialize DearImGui SDL2 backend"};
  }
}

DearImGuiVulkan::~DearImGuiVulkan()
{
  // Here we assume that the Vulkan backend has called ImGui_ImplVulkan_Init,
  // because the initialization has to be deferred after instance creation, etc.
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL2_Shutdown();
}

void DearImGuiVulkan::recreate_font_textures(VkCommandBuffer command_buffer)
{
  ImGui_ImplVulkan_DestroyFontUploadObjects();
  ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
}

}  // namespace gravel