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

class IAppDelegate {
 public:
  virtual ~IAppDelegate() noexcept = default;

  /** Initializes the state of the application. */
  virtual void initialize(IRenderDevice* render_device) = 0;

  /** Frees resources as needed just before the application is shut down. */
  virtual void dispose() = 0;

  /** Updates the state of the application. The provided delta time is in seconds. */
  virtual void tick(double delta) = 0;

  /** Renders the application scene. */
  virtual void render() = 0;

  /** Indicates whether the application is currently running. */
  [[nodiscard]] virtual auto is_running() const -> bool = 0;
};

}  // namespace glow
