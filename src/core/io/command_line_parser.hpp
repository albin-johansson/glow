#pragma once

#include <spdlog/spdlog.h>

#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/vector.hpp"
#include "graphics/graphics_api.hpp"

namespace glow {

using LogLevel = spdlog::level::level_enum;

struct CommandLineArgs final {
  LogLevel log_level {LogLevel::info};    ///< Log level to use.
  GraphicsApi api {GraphicsApi::OpenGL};  ///< The graphics backend to use.
  Maybe<Path> env_path;                   ///< Path to an environment texture to load.
  Vector<Path> model_paths;               ///< Paths to model files to load at startup.
};

[[nodiscard]] auto parse_command_line_args(int argc, char* argv[])
    -> Maybe<CommandLineArgs>;

}  // namespace glow
