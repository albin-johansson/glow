#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // set_terminate

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "engine/engine.hpp"
#include "graphics/opengl/opengl_backend.hpp"

using namespace gravel;

auto main(int, char*[]) -> int
{
  try {
    std::set_terminate(&on_terminate);

    spdlog::set_pattern("%^[%L][%T.%f]%$ %v");
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::critical);

    Engine engine {GraphicsApi::OpenGL};
    engine.set_backend(std::make_unique<gl::OpenGLBackend>(engine.get_window()));
    engine.start();

    return EXIT_SUCCESS;
  }
  catch (const Error& err) {
    spdlog::critical("Unhandled exception: {}\n{}", err.what(), err.trace());
    return EXIT_FAILURE;
  }
  catch (...) {
    spdlog::critical("Unhandled exception");
    return EXIT_FAILURE;
  }
}
