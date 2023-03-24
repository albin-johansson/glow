#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel {

class DearImGuiVulkan final {
 public:
  GRAVEL_DELETE_COPY(DearImGuiVulkan);
  GRAVEL_DELETE_MOVE(DearImGuiVulkan);

  explicit DearImGuiVulkan(SDL_Window* window);

  ~DearImGuiVulkan();

  void recreate_font_textures(VkCommandBuffer command_buffer);
};

}  // namespace gravel
