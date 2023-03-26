#include "util.hpp"

#include <magic_enum.hpp>

namespace gravel::vlk {

auto stringify_result(VkResult result) -> StringView
{
  return magic_enum::enum_name(result);
}

}  // namespace gravel::vlk
