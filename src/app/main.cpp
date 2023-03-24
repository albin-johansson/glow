#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // set_terminate

#include <SDL2/SDL.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "engine/backend.hpp"
#include "engine/engine.hpp"

namespace grv = gravel;

auto main(int, char*[]) -> int
{
  try {
    std::set_terminate(&grv::on_terminate);

    spdlog::set_pattern("%^[%L][%T.%f]%$ %v");
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::critical);

    const auto api = grv::GraphicsApi::OpenGL;
    spdlog::info("Using {} backend", magic_enum::enum_name(api));

    grv::Engine engine {api};
    engine.set_backend(create_backend(engine.get_window(), api));
    engine.start();

    return EXIT_SUCCESS;
  }
  catch (const grv::Error& err) {
    spdlog::critical("Unhandled exception: {}\n{}", err.what(), err.trace());
    return EXIT_FAILURE;
  }
  catch (...) {
    spdlog::critical("Unhandled exception");
    return EXIT_FAILURE;
  }
}
