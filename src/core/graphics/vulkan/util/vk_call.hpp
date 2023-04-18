#pragma once

#include <fmt/format.h>
#include <vulkan/vulkan.h>

#include "common/debug/error.hpp"
#include "common/type/string.hpp"

namespace glow::vk {

[[nodiscard]] auto stringify_result(VkResult result) -> StringView;

}  // namespace glow::vk

#define GLOW_VK_CALL(Expr, ErrorMsg)                                                     \
  do {                                                                                   \
    if (const auto res = (Expr); res != VK_SUCCESS) {                                    \
      throw glow::Error {fmt::format(ErrorMsg ": {}", glow::vk::stringify_result(res))}; \
    }                                                                                    \
  } while (false)
