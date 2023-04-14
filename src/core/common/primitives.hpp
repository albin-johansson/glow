#pragma once

#include <cstddef>  // byte, size_t, ptrdiff_t
#include <cstdint>  // uint{}_t, int{}_t, uintptr_t

namespace glow {

using Byte = std::byte;

using usize = std::size_t;
using ssize = std::ptrdiff_t;
using uintptr = std::uintptr_t;

using uchar = unsigned char;
using uint = unsigned int;
using ulonglong = unsigned long long int;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

static_assert(sizeof(float32) == 4);
static_assert(sizeof(float64) == 8);

}  // namespace glow