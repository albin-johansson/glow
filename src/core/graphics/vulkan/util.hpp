#pragma once

#include <vulkan/vulkan.h>

#include "common/type/array.hpp"

namespace gravel::vlk {

/// Names of the validations layers we use.
inline constexpr Array<const char*, 1> kValidationLayerNames {
    "VK_LAYER_KHRONOS_validation",
};

/// Names of the extensions that the Vulkan implementation must provide.
inline constexpr Array<const char*, 1> kRequiredExtensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

}  // namespace gravel::vlk
