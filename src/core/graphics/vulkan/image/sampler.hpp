#pragma once

#include <vulkan/vulkan.h>

#include "common/type/memory.hpp"

namespace glow::vk {

struct SamplerDeleter final {
  void operator()(VkSampler sampler) noexcept;
};

using Sampler = Unique<VkSampler_T, SamplerDeleter>;

[[nodiscard]] auto create_sampler() -> Sampler;

}  // namespace glow::vk
