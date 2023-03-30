#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

class CommandPool final {
 public:
  GRAVEL_DELETE_COPY(CommandPool);

  CommandPool();

  ~CommandPool() noexcept;

  CommandPool(CommandPool&& other) noexcept;

  auto operator=(CommandPool&& other) noexcept -> CommandPool&;

  [[nodiscard]] auto create_command_buffer() -> VkCommandBuffer;

  [[nodiscard]] auto create_command_buffers(uint32 count) -> Vector<VkCommandBuffer>;

  [[nodiscard]] auto get() -> VkCommandPool { return mCommandPool; }

 private:
  VkCommandPool mCommandPool {VK_NULL_HANDLE};

  void dispose() noexcept;
};

}  // namespace gravel::vlk
