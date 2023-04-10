#include "sampler.hpp"

#include "graphics/vulkan/context.hpp"
#include "graphics/vulkan/util/vk_call.hpp"

namespace gravel::vk {

Sampler::Sampler()
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

  GRAVEL_VK_CALL(vkCreateSampler(get_device(), &create_info, nullptr, &mSampler),
                 "[VK] Could not create sampler");
}

Sampler::~Sampler() noexcept
{
  dispose();
}

void Sampler::dispose() noexcept
{
  if (mSampler != VK_NULL_HANDLE) {
    vkDestroySampler(get_device(), mSampler, nullptr);
  }
}

Sampler::Sampler(Sampler&& other) noexcept
    : mSampler {other.mSampler}
{
  other.mSampler = VK_NULL_HANDLE;
}

auto Sampler::operator=(Sampler&& other) noexcept -> Sampler&
{
  if (this != &other) {
    dispose();

    mSampler = other.mSampler;
    other.mSampler = VK_NULL_HANDLE;
  }

  return *this;
}

}  // namespace gravel::vk
