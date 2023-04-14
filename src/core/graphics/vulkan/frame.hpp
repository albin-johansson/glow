#pragma once

#include <vulkan/vulkan.h>

#include "graphics/vulkan/buffer.hpp"
#include "graphics/vulkan/shader_buffers.hpp"
#include "graphics/vulkan/sync/fence.hpp"
#include "graphics/vulkan/sync/semaphore.hpp"

namespace glow::vk {

/// Stores resources required to allow for multiple frames in flight at the same time.
struct FrameData final {
  VkCommandBuffer command_buffer {VK_NULL_HANDLE};

  Semaphore image_available_semaphore {create_semaphore()};
  Semaphore render_finished_semaphore {create_semaphore()};
  Fence in_flight_fence {create_fence(true)};

  Buffer static_matrix_ubo {Buffer::uniform(sizeof(StaticMatrices))};
  Buffer material_ubo {Buffer::uniform(sizeof(MaterialBuffer))};
};

}  // namespace glow::vk
