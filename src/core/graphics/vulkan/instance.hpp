#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "common/predef.hpp"
#include "common/type/vector.hpp"

namespace gravel::vlk {

/// RAII wrapper for a `VkInstance`.
class Instance final {
 public:
  GRAVEL_DELETE_COPY(Instance);
  GRAVEL_DELETE_MOVE(Instance);

  /// Initializes a Vulkan instance.
  Instance();

  /// Destroys the associated Vulkan instance.
  ~Instance() noexcept;

  [[nodiscard]] auto get() -> VkInstance { return mInstance; }

 private:
  VkInstance mInstance {VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT mDebugMessenger {VK_NULL_HANDLE};

  void create_debug_messenger();
};

/// Returns all available GPUs, regardless of their suitability.
[[nodiscard]] auto get_physical_devices(VkInstance instance) -> Vector<VkPhysicalDevice>;

}  // namespace gravel::vlk
