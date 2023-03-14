#include <algorithm>  // min
#include <cstdlib>    // EXIT_FAILURE, EXIT_SUCCESS

#include <SDL2/SDL.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/primitives.hpp"
#include "common/type/math.hpp"
#include "graphics/opengl/context.hpp"
#include "graphics/opengl/opengl_backend.hpp"
#include "graphics/opengl/texture_cache.hpp"
#include "init/dear_imgui.hpp"
#include "init/sdl.hpp"
#include "init/window.hpp"
#include "scene/scene.hpp"

using namespace gravel;

[[nodiscard]] static auto determine_refresh_rate() -> float64
{
  SDL_DisplayMode mode {};
  if (SDL_GetDesktopDisplayMode(0, &mode) == 0) {
    return std::min(120, mode.refresh_rate);
  }
  else {
    spdlog::warn("[Engine] Failed to determine screen refresh rate, assuming 60 Hz");
    return 60;
  }
}

auto main(int, char*[]) -> int
{
  try {
    spdlog::set_pattern("%^[%L][%T.%f]%$ %v");
    spdlog::set_level(spdlog::level::debug);

    const auto api = GraphicsApi::OpenGL;

    const SDL sdl {api};
    Window window {api};
    auto* window_handle = window.get_handle();

    const gl::Context context {window_handle};
    DearImGui imgui {window_handle, api};

    const auto counter_freq = static_cast<float64>(SDL_GetPerformanceFrequency());
    const auto refresh_rate = determine_refresh_rate();
    const float64 fixed_delta = 1.0 / refresh_rate;
    const int32 max_ticks_per_frame = 5;

    spdlog::debug("[Engine] Fixed delta: {:.4f}", fixed_delta);
    spdlog::debug("[Engine] Counter frequency: {:L}", counter_freq);
    spdlog::debug("[Engine] Max ticks per frame: {}", max_ticks_per_frame);

    auto query_counter = [=]() -> float64 {
      return static_cast<float64>(SDL_GetPerformanceCounter()) / counter_freq;
    };

    float64 last_update = query_counter();
    Vec2 last_framebuffer_scale {};

    SDL_MaximizeWindow(window_handle);
    SDL_ShowWindow(window_handle);

    Scene scene;
    const auto model_entity = scene.make_node();

    auto& ctx = scene.get_registry().ctx();
    ctx.emplace<gl::TextureCache>();

    gl::OpenGLBackend backend {window_handle};

    // The following is a semi-fixed delta time game loop implementation.
    // Logic updates are detached from rendered frames, that is, there may be multiple
    // logic updates for each render call.
    while (!backend.should_quit()) {
      const auto new_time = query_counter();

      auto frame_time = new_time - last_update;
      last_update = new_time;

      int32 ticks = 0;
      while (frame_time > 0) {
        // Avoids spiral of death
        if (ticks > max_ticks_per_frame) {
          break;
        }

        if (backend.should_quit()) {
          break;
        }

        const auto delta = std::min(frame_time, fixed_delta);
        backend.update(static_cast<float32>(delta));

        frame_time -= delta;
        ++ticks;
      }

      const auto& io = ImGui::GetIO();
      const Vec2 fb_scale {io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y};

      if (fb_scale != last_framebuffer_scale) {
        imgui.reload_fonts();
        last_framebuffer_scale = fb_scale;
      }

      backend.render(scene);
    }

    SDL_HideWindow(window_handle);
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
