#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"

namespace gravel::vk {

/// Wrapper around the dedicated Vulkan memory allocator.
///
/// Note, the instance, physical device, and logical device must all have been initialized
/// before creating an allocator.
class Allocator final {
 public:
  GRAVEL_DELETE_COPY(Allocator);
  GRAVEL_DELETE_MOVE(Allocator);

  Allocator();

  ~Allocator() noexcept;

  [[nodiscard]] auto get() -> VmaAllocator { return mAllocator; }

 private:
  VmaAllocator mAllocator;
};

}  // namespace gravel::vk
