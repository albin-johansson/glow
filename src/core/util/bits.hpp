#pragma once

#include <cstring>      // memcpy
#include <type_traits>  // is_trivially_copyable_v, is_trivially_constructible_v

namespace glow {

template <typename To, typename From>
  requires(sizeof(To) == sizeof(From) &&          //
           std::is_trivially_copyable_v<From> &&  //
           std::is_trivially_copyable_v<To> &&    //
           std::is_trivially_constructible_v<To>)
[[nodiscard]] auto bitcast(const From& from) noexcept -> To
{
  To to;
  std::memcpy(&to, &from, sizeof(To));
  return to;
}

}  // namespace glow
