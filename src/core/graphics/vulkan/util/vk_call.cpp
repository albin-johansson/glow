#include "vk_call.hpp"

#include <magic_enum.hpp>

namespace glow::vk {

auto stringify_result(const VkResult result) -> StringView
{
  return magic_enum::enum_name(result);
}

}  // namespace glow::vk
