/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/rhi.hpp"

namespace glow {

enum class ImageLayout : uint32 {
  Undefined = 0,
  ColorAttachment,  ///< Optimal for use as color attachment.
  ShaderRead,       ///< Optimal for read access in shaders.
  Presentation,     ///< Optimal for presentation to screen.
  CopySrc,          ///< Optimal for use as copy source.
  CopyDst,          ///< Optimal for use as copy destination.
};

class GLOW_CORE_API IImage {
 public:
  virtual ~IImage() noexcept = default;

  virtual void generate_mipmaps() = 0;

  virtual void change_layout(ImageLayout new_layout) = 0;

  [[nodiscard]] virtual auto get_layout() const -> ImageLayout = 0;

  [[nodiscard]] virtual auto get_format() const -> PixelFormat = 0;

  [[nodiscard]] virtual auto get_width(uint32 level) const -> uint32 = 0;

  [[nodiscard]] virtual auto get_height(uint32 level) const -> uint32 = 0;

  [[nodiscard]] virtual auto get_level_count() const -> uint32 = 0;
};

}  // namespace glow
