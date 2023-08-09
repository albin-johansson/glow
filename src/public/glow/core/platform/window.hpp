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

#include <memory>  // unique_ptr

#include "glow/core/prelude.hpp"
#include "glow/core/rhi/graphics_api.hpp"

namespace glow {

class GLOW_CORE_API Window final {
 public:
  GLOW_CANNOT_COPY(Window);

  explicit Window(GraphicsAPI api);

  ~Window() noexcept;

  Window(Window&& other) noexcept;

  auto operator=(Window&& other) noexcept -> Window&;

  void show();

  void hide();

  [[nodiscard]] auto get_width() const -> uint32;

  [[nodiscard]] auto get_height() const -> uint32;

  [[nodiscard]] auto get_graphics_api() const -> GraphicsAPI;

  [[nodiscard]] auto get_handle() -> void*;

 private:
  struct Data;
  std::unique_ptr<Data> mData;
};

}  // namespace glow
