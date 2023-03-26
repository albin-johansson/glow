#pragma once

#include <array>  // array

#include <fmt/format.h>
#include <vulkan/vulkan.h>

#include "common/debug/error.hpp"
#include "common/predef.hpp"
#include "common/type/array.hpp"
#include "common/type/string.hpp"

namespace gravel::vlk {

/// Names of the validations layers we use.
inline constexpr Array<const char*, 1> kValidationLayerNames {
    "VK_LAYER_KHRONOS_validation",
};

/// Names of the extensions that the Vulkan implementation must provide.
/// TODO: use CTAD with the Array alias here when all compilers support it
inline constexpr std::array kRequiredDeviceExtensions
{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if GRAVEL_OS_MACOS
  "VK_KHR_portability_subset",
#endif  // GRAVEL_OS_MACOS
};

[[nodiscard]] auto stringify_result(VkResult result) -> StringView;

}  // namespace gravel::vlk

#define GRAVEL_VK_CALL(Expr, ErrorMsg)                                                \
  do {                                                                                \
    if (const auto res = (Expr); res != VK_SUCCESS) {                                 \
      throw Error {fmt::format(ErrorMsg ": {}", gravel::vlk::stringify_result(res))}; \
    }                                                                                 \
  } while (false)
