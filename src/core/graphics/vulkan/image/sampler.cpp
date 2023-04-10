#include "sampler.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

void SamplerDeleter::operator()(VkSampler sampler) noexcept
{
  vkDestroySampler(get_device(), sampler, nullptr);
}

auto create_sampler() -> Sampler
{
  VkPhysicalDeviceFeatures device_features {};
  vkGetPhysicalDeviceFeatures(get_gpu(), &device_features);

  VkPhysicalDeviceProperties device_properties {};
  vkGetPhysicalDeviceProperties(get_gpu(), &device_properties);

  const VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  const VkSamplerCreateInfo create_info {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,

      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,

      .addressModeU = address_mode,
      .addressModeV = address_mode,
      .addressModeW = address_mode,

      .mipLodBias = 0.0f,

      .anisotropyEnable = device_features.samplerAnisotropy,
      .maxAnisotropy = device_properties.limits.maxSamplerAnisotropy,

      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,

      .minLod = 0.0f,
      .maxLod = 0.0f,

      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
  };

  VkSampler sampler = VK_NULL_HANDLE;
  GRAVEL_VK_CALL(vkCreateSampler(get_device(), &create_info, nullptr, &sampler),
                 "[VK] Could not create sampler");

  return Sampler {sampler};
}

}  // namespace gravel::vk
