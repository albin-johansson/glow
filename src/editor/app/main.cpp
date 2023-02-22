#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // exception

#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <spdlog/spdlog.h>

auto main(int, char*[]) -> int
{
  try {
  }
  catch (const std::exception& e) {
    spdlog::critical("Unhandled exception: {}", e.what());
    return EXIT_FAILURE;
  }
  catch (...) {
    spdlog::critical("Unhandled exception");
    return EXIT_FAILURE;
  }
}
