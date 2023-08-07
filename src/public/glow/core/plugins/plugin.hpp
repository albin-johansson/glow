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

#include <filesystem>   // path
#include <memory>       // unique_ptr
#include <type_traits>  // is_function_v

#include "glow/core/prelude.hpp"

namespace glow {

class GLOW_CORE_API Plugin final {
 public:
  GLOW_CANNOT_COPY(Plugin);

  Plugin();

  ~Plugin() noexcept;

  Plugin(Plugin&& other) noexcept;

  auto operator=(Plugin&& other) noexcept -> Plugin&;

  void load(const std::filesystem::path& file);

  [[nodiscard]] auto is_rhi_plugin() const -> bool;

  [[nodiscard]] auto is_valid() const -> bool;

  template <typename T>
    requires std::is_function_v<T>
  [[nodiscard]] auto get_function(const char* name) const -> T*
  {
    return reinterpret_cast<T*>(_get_symbol(name));
  }

 private:
  struct Data;
  std::unique_ptr<Data> mData;

  [[nodiscard]] auto _get_symbol(const char* name) const -> void*;
};

}  // namespace glow
