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

#include <functional>  // hash

#include "glow/core/prelude.hpp"

namespace glow {

template <typename T>
void hash_combine(usize& seed, const T& val)
{
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0814r0.pdf
  seed ^= std::hash<T> {}(val) + 0x9E3779B9u + (seed << 6u) + (seed >> 2u);
}

[[nodiscard]] auto hash_combine(const auto&... args) -> usize
{
  usize seed = 0;
  (hash_combine(seed, args), ...);
  return seed;
}

}  // namespace glow