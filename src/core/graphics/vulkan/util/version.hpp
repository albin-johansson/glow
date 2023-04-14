#pragma once

#include <vulkan/vulkan.h>

#include "common/primitives.hpp"
#include "common/type/string.hpp"

namespace glow::vk {

[[nodiscard]] auto to_version_string(uint32 version) -> String;

[[nodiscard]] auto get_api_version(VkPhysicalDevice gpu) -> String;

[[nodiscard]] auto get_driver_version(VkPhysicalDevice gpu) -> String;

}  // namespace glow::vk
