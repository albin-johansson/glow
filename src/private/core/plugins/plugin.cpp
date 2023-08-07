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

#include "glow/core/plugins/plugin.hpp"

#include <SDL2/SDL.h>

namespace glow {
namespace {

struct SharedLibraryDeleter final {
  void operator()(void* handle) noexcept { SDL_UnloadObject(handle); }
};

}  // namespace

using SharedLibraryHandle = std::unique_ptr<void, SharedLibraryDeleter>;

struct Plugin::Data final {
  SharedLibraryHandle dll;
};

Plugin::Plugin()
    : mData {std::make_unique<Data>()}
{
}

Plugin::~Plugin() noexcept = default;

Plugin::Plugin(Plugin&& other) noexcept = default;

auto Plugin::operator=(Plugin&& other) noexcept -> Plugin& = default;

void Plugin::load(const std::filesystem::path& file)
{
  mData->dll.reset(SDL_LoadObject(file.string().c_str()));
}

auto Plugin::is_rhi_plugin() const -> bool
{
  if (is_valid()) {
    return _get_symbol("glow_rhi_init") && _get_symbol("glow_rhi_get_name");
  }

  return false;
}

auto Plugin::is_valid() const -> bool
{
  return mData->dll != nullptr;
}

auto Plugin::_get_symbol(const char* name) const -> void*
{
  if (is_valid()) {
    return SDL_LoadFunction(mData->dll.get(), name);
  }

  return nullptr;
}

}  // namespace glow
