#include "dear_imgui_opengl.hpp"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "common/debug/error.hpp"

namespace glow {

DearImGuiOpenGL::DearImGuiOpenGL(SDL_Window* window)
{
  if (!ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext())) {
    throw Error {"[GUI] Could not initialize DearImGui SDL2 backend"};
  }

  if (!ImGui_ImplOpenGL3_Init("#version 410 core")) {
    throw Error {"[GUI] Could not initialize DearImGui OpenGL backend"};
  }
}

DearImGuiOpenGL::~DearImGuiOpenGL()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
}

void DearImGuiOpenGL::recreate_font_textures()
{
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();
}

}  // namespace glow