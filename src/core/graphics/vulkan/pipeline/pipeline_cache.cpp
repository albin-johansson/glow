#include "pipeline_cache.hpp"

#include "common/debug/assert.hpp"
#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util.hpp"

namespace gravel::vk {

PipelineCache::PipelineCache()
{
  GRAVEL_ASSERT(get_device() != VK_NULL_HANDLE);

  const VkPipelineCacheCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .initialDataSize = 0,
      .pInitialData = nullptr,
  };

  GRAVEL_VK_CALL(vkCreatePipelineCache(get_device(), &create_info, nullptr, &mCache),
                 "[VK] Could not create pipeline cache");
}

PipelineCache::~PipelineCache() noexcept
{
  dispose();
}

void PipelineCache::dispose() noexcept
{
  if (mCache != VK_NULL_HANDLE) {
    vkDestroyPipelineCache(get_device(), mCache, nullptr);
  }
}

PipelineCache::PipelineCache(PipelineCache&& other) noexcept
    : mCache {other.mCache}
{
  other.mCache = VK_NULL_HANDLE;
}

auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache&
{
  if (this != &other) {
    dispose();

    mCache = other.mCache;
    other.mCache = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vk
