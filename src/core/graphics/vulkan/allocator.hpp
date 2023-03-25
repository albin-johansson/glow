#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vlk {

class Allocator final {
 public:
  GRAVEL_DELETE_COPY(Allocator);
  GRAVEL_DELETE_MOVE(Allocator);

  Allocator(VkInstance instance, VkPhysicalDevice gpu, VkDevice device);

  ~Allocator();

 private:
  VmaAllocator mAllocator;
};

}  // namespace gravel::vlk
