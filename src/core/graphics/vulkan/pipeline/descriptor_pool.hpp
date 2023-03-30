#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vlk {

class DescriptorPool final {
 public:
  GRAVEL_DELETE_COPY(DescriptorPool);

  DescriptorPool(const VkDescriptorPoolSize* pool_sizes, uint32 pool_size_count);

  ~DescriptorPool() noexcept;

  DescriptorPool(DescriptorPool&& other) noexcept;

  auto operator=(DescriptorPool&& other) noexcept -> DescriptorPool&;

  [[nodiscard]] auto get() -> VkDescriptorPool { return mPool; }

 private:
  VkDescriptorPool mPool {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
