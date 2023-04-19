#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // set_terminate

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "engine/backend.hpp"
#include "engine/engine.hpp"
#include "io/command_line_parser.hpp"

auto main(const int argc, char* argv[]) -> int
{
  try {
    std::set_terminate(&glow::on_terminate);

    spdlog::set_pattern("%^[%L][%T.%e]%$ %v");
    spdlog::flush_on(spdlog::level::critical);

    const auto command_line_args = glow::parse_command_line_args(argc, argv);
    if (!command_line_args) {
      return EXIT_FAILURE;
    }

    const auto log_level = command_line_args->log_level;
    spdlog::set_level(log_level);

    const auto api = command_line_args->api;
    spdlog::info("[Main] Using {}", get_short_name(api));

    glow::Engine engine {api};
    engine.set_backend(glow::create_backend(engine.get_window(), api));
    engine.init();

    if (command_line_args->env_path) {
      spdlog::debug("[Main] Specified environment texture '{}'",
                    command_line_args->env_path->string());
      engine.set_environment_texture(*command_line_args->env_path);
    }

    for (const auto& model_path : command_line_args->model_paths) {
      engine.load_model(model_path);
    }

    engine.start();
    return EXIT_SUCCESS;
  }
  catch (const glow::Error& err) {
    spdlog::critical("[Main] Unhandled exception: {}\n{}", err.what(), err.trace());
    return EXIT_FAILURE;
  }
  catch (...) {
    spdlog::critical("[Main] Unhandled exception");
    return EXIT_FAILURE;
  }
}
