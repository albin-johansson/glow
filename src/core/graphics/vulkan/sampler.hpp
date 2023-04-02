#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class Sampler final {
 public:
  GRAVEL_DELETE_COPY(Sampler);

  Sampler();

  ~Sampler() noexcept;

  Sampler(Sampler&& other) noexcept;

  auto operator=(Sampler&& other) noexcept -> Sampler&;

  [[nodiscard]] auto get() noexcept -> VkSampler { return mSampler; }

 private:
  VkSampler mSampler {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
