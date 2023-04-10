#pragma once

#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel::vlk {

class Device final {
 public:
  GRAVEL_DELETE_COPY(Device);
  GRAVEL_DELETE_MOVE(Device);

  Device();

  ~Device();

  [[nodiscard]] auto get() -> VkDevice { return mDevice; }
  [[nodiscard]] auto get_graphics_queue() -> VkQueue { return mGraphicsQueue; }
  [[nodiscard]] auto get_present_queue() -> VkQueue { return mPresentQueue; }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkQueue mGraphicsQueue {VK_NULL_HANDLE};
  VkQueue mPresentQueue {VK_NULL_HANDLE};
};

}  // namespace gravel::vlk
