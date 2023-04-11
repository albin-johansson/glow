#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/memory.hpp"
#include "common/type/vector.hpp"

namespace gravel::vk {

struct CommandPoolDeleter final {
  void operator()(VkCommandPool pool) noexcept;
};

using ManagedCommandPool = Unique<VkCommandPool_T, CommandPoolDeleter>;

class CommandPool final {
 public:
  GRAVEL_DELETE_COPY(CommandPool);
  GRAVEL_DEFAULT_MOVE(CommandPool);

  CommandPool();

  [[nodiscard]] auto allocate_command_buffer() -> VkCommandBuffer;

  [[nodiscard]] auto allocate_command_buffers(uint32 count) -> Vector<VkCommandBuffer>;

  [[nodiscard]] auto get() noexcept -> VkCommandPool { return mCommandPool.get(); }

 private:
  ManagedCommandPool mCommandPool;
};

[[nodiscard]] auto allocate_command_buffer(VkCommandPool command_pool) -> VkCommandBuffer;

[[nodiscard]] auto allocate_command_buffers(VkCommandPool command_pool, uint32 count)
    -> Vector<VkCommandBuffer>;

}  // namespace gravel::vk
