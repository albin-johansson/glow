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

#include "glow/core/plugins/plugin_manager.hpp"

#include <filesystem>  // directory_iterator
#include <iostream>    // clog
#include <utility>     // move

#include "glow/core/foundation/error.hpp"

namespace glow {

void PluginManager::scan(const std::filesystem::path& dir)
{
  for (const auto& entry : std::filesystem::directory_iterator {dir}) {
    const auto& entry_path = entry.path();

    const auto extension = entry_path.extension();
    if (extension == ".so" || extension == ".dll") {
      Plugin plugin;
      plugin.load(entry_path);

      if (plugin.is_valid()) {
        std::clog << "Loading plugin " << entry_path.string() << '\n';

        if (plugin.is_rhi_plugin()) {
          mRHIPlugins.push_back(plugin.get_rhi_plugin_functions());
        }

        mPlugins.push_back(std::move(plugin));
      }
    }
  }
}

auto PluginManager::get_rhi(const GraphicsAPI api) const -> RHIPluginFunctions
{
  for (const auto& rhi_plugin : mRHIPlugins) {
    if (rhi_plugin.get_api() == api) {
      return rhi_plugin;
    }
  }

  throw Error {"No loaded plugin for requested graphics API"};
}

auto PluginManager::has_rhi(const GraphicsAPI api) const -> bool
{
  for (const auto& rhi_plugin : mRHIPlugins) {
    if (rhi_plugin.get_api() == api) {
      return true;
    }
  }

  return false;
}

}  // namespace glow
