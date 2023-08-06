#pragma once

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

namespace glow {

enum class PixelFormat : uint32 {
  Undefined = 0,

  U8_R,
  U8_RG,
  U8_RGB,
  U8_RGB_SRGB,
  U8_RGBA,
  U8_RGBA_SRGB,

  U8_BGR,
  U8_BGR_SRGB,
  U8_BGRA,
  U8_BGRA_SRGB,

  F32_R,
  F32_RG,
  F32_RGB,
  F32_RGBA,

  U16_DEPTH,
  U16_DEPTH_U8_STENCIL,
  U24_DEPTH_U8_STENCIL,
  F32_DEPTH,
  F32_DEPTH_U8_STENCIL,
};

/** Indicates whether a pixel format represents a depth buffer format. */
[[nodiscard]] GLOW_CORE_API auto is_depth_format(PixelFormat format) -> bool;

/** Indicates whether a pixel format features a stencil component. */
[[nodiscard]] GLOW_CORE_API auto has_stencil_component(PixelFormat format) -> bool;

}  // namespace glow
