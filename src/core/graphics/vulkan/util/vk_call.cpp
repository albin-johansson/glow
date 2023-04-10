#include "vk_call.hpp"

#include <magic_enum.hpp>

namespace gravel::vk {

auto stringify_result(const VkResult result) -> StringView
{
  return magic_enum::enum_name(result);
}

}  // namespace gravel::vk
