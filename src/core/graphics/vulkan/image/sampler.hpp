#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct SamplerDeleter final {
  void operator()(VkSampler sampler) noexcept;
};

using SamplerPtr = Unique<VkSampler_T, SamplerDeleter>;

[[nodiscard]] auto create_sampler(VkSamplerAddressMode address_mode) -> SamplerPtr;

}  // namespace glow::vk
