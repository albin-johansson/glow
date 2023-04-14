#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace glow::vk {

class DescriptorPool final {
 public:
  GRAVEL_DELETE_COPY(DescriptorPool);

  DescriptorPool(uint32 max_sets,
                 const VkDescriptorPoolSize* pool_sizes,
                 uint32 pool_size_count,
                 VkDescriptorPoolCreateFlags flags = 0);

  ~DescriptorPool() noexcept;

  DescriptorPool(DescriptorPool&& other) noexcept;

  auto operator=(DescriptorPool&& other) noexcept -> DescriptorPool&;

  [[nodiscard]] auto allocate(VkDescriptorSetLayout layout) -> VkDescriptorSet;

  [[nodiscard]] auto get() -> VkDescriptorPool { return mPool; }

 private:
  VkDescriptorPool mPool {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace glow::vk
