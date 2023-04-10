#pragma once

#include <array>  // array

#include <fmt/format.h>
#include <vulkan/vulkan.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "common/type/string.hpp"

namespace gravel::vk {

inline constexpr usize kMaxFramesInFlight = 2;

inline constexpr uint32 kGlobalDescriptorSet = 0;
inline constexpr uint32 kMaterialDescriptorSet = 1;

inline constexpr uint32 kDescriptorSetCount = 2;

/// Names of the validations layers we use.
inline constexpr const char* kValidationLayerNames[] {
    "VK_LAYER_KHRONOS_validation",
};

/// Names of the extensions that the Vulkan implementation must provide.
inline constexpr const char* kRequiredDeviceExtensions[] {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,

#ifdef GRAVEL_USE_VULKAN_SUBSET
    "VK_KHR_portability_subset",
#endif  // GRAVEL_USE_VULKAN_SUBSET
};

[[nodiscard]] auto stringify_result(VkResult result) -> StringView;

[[nodiscard]] auto get_api_version(VkPhysicalDevice gpu) -> String;

[[nodiscard]] auto get_driver_version(VkPhysicalDevice gpu) -> String;

}  // namespace gravel::vk

#define GRAVEL_VK_CALL(Expr, ErrorMsg)                                                \
  do {                                                                                \
    if (const auto res = (Expr); res != VK_SUCCESS) {                                 \
      throw Error {fmt::format(ErrorMsg ": {}", gravel::vk::stringify_result(res))}; \
    }                                                                                 \
  } while (false)
