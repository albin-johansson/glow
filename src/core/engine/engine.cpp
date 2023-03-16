#include "engine.hpp"

#include <algorithm>  // min
#include <utility>    // move

#include <imgui.h>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"
#include "common/type/math.hpp"
#include "engine/backend.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto determine_refresh_rate() -> float64
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

}  // namespace

Engine::Engine(const GraphicsApi api)
    : mInitializer {api},
      mWindow {mInitializer.get_window().get_handle()},
      mCounterFreq(static_cast<float64>(SDL_GetPerformanceFrequency())),
      mFixedDelta {1.0 / determine_refresh_rate()}
{
  spdlog::debug("[Engine] Counter frequency: {:L}", mCounterFreq);
  spdlog::debug("[Engine] Fixed delta: {:.4f}", mFixedDelta);
  spdlog::debug("[Engine] Max ticks per frame: {}", mMaxTicksPerFrame);

  SDL_MaximizeWindow(mWindow);
  SDL_ShowWindow(mWindow);
}

void Engine::start()
{
  if (!mBackend) {
    throw Error {"[Engine] Backend instance was not set"};
  }

  mBackend->on_init(mScene);

  auto last_update = query_counter();
  Vec2 last_framebuffer_scale {};

  // The following is a semi-fixed delta time game loop implementation.
  // Logic updates are detached from rendered frames, that is, there may be multiple
  // logic updates for each render call.
  while (!mBackend->should_quit()) {
    const auto new_time = query_counter();

    auto frame_time = new_time - last_update;
    last_update = new_time;

    int32 ticks = 0;
    while (frame_time > 0) {
      if (ticks > mMaxTicksPerFrame) {
        // Avoids spiral of death
        break;
      }

      if (mBackend->should_quit()) {
        break;
      }

      const auto delta = std::min(frame_time, mFixedDelta);
      mBackend->on_update(static_cast<float32>(delta));

      frame_time -= delta;
      ++ticks;
    }

    const auto& io = ImGui::GetIO();
    const Vec2 fb_scale {io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y};

    if (fb_scale != last_framebuffer_scale) {
      mInitializer.get_imgui().reload_fonts();
      last_framebuffer_scale = fb_scale;
    }

    mBackend->on_render(mScene);
  }

  SDL_HideWindow(mWindow);
}

void Engine::set_backend(std::unique_ptr<Backend> backend)
{
  mBackend = std::move(backend);
}

auto Engine::query_counter() const -> float64
{
  return static_cast<float64>(SDL_GetPerformanceCounter()) / mCounterFreq;
}

}  // namespace gravel