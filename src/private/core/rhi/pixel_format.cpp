#include "glow/core/rhi/pixel_format.hpp"

namespace glow {

auto is_depth_format(const PixelFormat format) -> bool
{
  switch (format) {
    case PixelFormat::U16_DEPTH:
    case PixelFormat::U16_DEPTH_U8_STENCIL:
    case PixelFormat::U24_DEPTH_U8_STENCIL:
    case PixelFormat::F32_DEPTH:
    case PixelFormat::F32_DEPTH_U8_STENCIL:
      return true;

    default:
      return false;
  }
}

auto has_stencil_component(const PixelFormat format) -> bool
{
  switch (format) {
    case PixelFormat::U16_DEPTH_U8_STENCIL:
    case PixelFormat::U24_DEPTH_U8_STENCIL:
    case PixelFormat::F32_DEPTH_U8_STENCIL:
      return true;

    default:
      return false;
  }
}

}  // namespace glow
