#include "window.hpp"

#include <fmt/format.h>
#include <magic_enum.hpp>

#include "common/debug/error.hpp"
#include "common/predef.hpp"
#include "common/primitives.hpp"

#if GRAVEL_OS_WINDOWS

#include <SDL2/SDL_syswm.h>
#include <dwmapi.h>

#endif  // GRAVEL_OS_WINDOWS

namespace gravel {
namespace {

struct ObjectDeleter final {
  void operator()(void* obj) noexcept { SDL_UnloadObject(obj); }
};

[[nodiscard]] auto to_window_flags(const GraphicsApi api) -> uint32
{
  const auto base_flags =
      SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
  switch (api) {
    case GraphicsApi::OpenGL:
      return base_flags | SDL_WINDOW_OPENGL;

    case GraphicsApi::Vulkan:
      return base_flags | SDL_WINDOW_VULKAN;

    default:
      throw Error {"Invalid graphics API"};
  }
}

void use_win32_dark_title_bar(SDL_Window* window)
{
#if GRAVEL_OS_WINDOWS
  SDL_SysWMinfo wm_info {};
  SDL_VERSION(&wm_info.version);

  if (SDL_GetWindowWMInfo(window, &wm_info)) {
    Unique<void, ObjectDeleter> dwmapi {SDL_LoadObject("dwmapi.dll")};

    using Signature = HRESULT (*)(HWND, DWORD, LPCVOID, DWORD);
    if (auto* func = reinterpret_cast<Signature>(
            SDL_LoadFunction(dwmapi.get(), "DwmSetWindowAttribute"))) {
      HWND hwnd = wm_info.info.win.window;
      BOOL mode = 1;
      func(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &mode, sizeof mode);
    }
  }
#endif  // GRAVEL_OS_WINDOWS
}

}  // namespace

void WindowDeleter::operator()(SDL_Window* window) noexcept
{
  SDL_DestroyWindow(window);
}

Window::Window(const GraphicsApi api)
    : mWindow {SDL_CreateWindow("Gravel",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                1000,
                                750,
                                to_window_flags(api))}
{
  if (!mWindow) {
    throw Error {fmt::format("Could not create window: {}", SDL_GetError())};
  }

  use_win32_dark_title_bar(mWindow.get());

  const auto title = fmt::format("Gravel [{}]", magic_enum::enum_name(api));
  SDL_SetWindowTitle(mWindow.get(), title.c_str());
}

}  // namespace gravel
