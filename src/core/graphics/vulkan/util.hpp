#pragma once

#include <array>  // array

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/array.hpp"

namespace gravel::vlk {

/// Names of the validations layers we use.
inline constexpr Array<const char*, 1> kValidationLayerNames {
    "VK_LAYER_KHRONOS_validation",
};

/// Names of the extensions that the Vulkan implementation must provide.
/// TODO: use CTAD with the Array alias here when all compilers support it
inline constexpr std::array kRequiredExtensions
{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if GRAVEL_OS_MACOS
  "VK_KHR_portability_subset",
#endif  // GRAVEL_OS_MACOS
};

}  // namespace gravel::vlk
