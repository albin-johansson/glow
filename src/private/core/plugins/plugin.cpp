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

#include <boost/dll.hpp>

namespace glow {

using SharedLibrary = boost::dll::shared_library;

struct Plugin::Data final {
  SharedLibrary dll;
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
  mData->dll = SharedLibrary {file};
}

auto Plugin::is_rhi_plugin() const -> bool
{
  return mData->dll.has("glow_rhi_init") && mData->dll.has("glow_rhi_get_name");
}

auto Plugin::is_valid() const -> bool
{
  return mData->dll.is_loaded();
}

auto Plugin::_get_symbol(const char* name) const -> void*
{
  return mData->dll.get<void*>(name);
}

}  // namespace glow
