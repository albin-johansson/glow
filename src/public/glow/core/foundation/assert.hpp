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

#ifndef GLOW_ENABLE_ASSERTIONS
  #ifdef NDEBUG
    #define GLOW_ENABLE_ASSERTIONS 0
  #else
    #define GLOW_ENABLE_ASSERTIONS 1
  #endif  // NDEBUG
#endif    // GLOW_ENABLE_ASSERTIONS

#if GLOW_ENABLE_ASSERTIONS
  #define GLOW_ASSERT_MSG(Expr, Msg) \
    (!!(Expr) ? ((void) 0) : glow::on_assertion_failed(#Expr, (Msg), __FILE__, __LINE__))
  #define GLOW_ASSERT(Expr) GLOW_ASSERT_MSG(Expr, "?")
#else
  #define GLOW_ASSERT(Expr)
  #define GLOW_ASSERT_MSG(Expr, Msg)
#endif  // GLOW_ENABLE_ASSERTIONS

namespace glow {

GLOW_CORE_API void on_assertion_failed(const char* expr,
                                       const char* msg,
                                       const char* file,
                                       int64 line);

}  // namespace glow
