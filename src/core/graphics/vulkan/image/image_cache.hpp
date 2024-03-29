#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/map.hpp"
#include "common/type/path.hpp"
#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/image/image_view.hpp"

namespace glow::vk {

/// Vulkan image cache context component.
struct ImageCache final {
  GLOW_MOVE_ONLY_COMPONENT(ImageCache);

  Map<Path, Image> images;
  Map<VkImage, ImageViewPtr> views;
};

}  // namespace glow::vk
