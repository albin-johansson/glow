#include "menu_bar.hpp"

#include <imgui.h>

#include "ui/events.hpp"

namespace gravel {

void show_menu_bar(Dispatcher& dispatcher)
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {

      if (ImGui::MenuItem("Exit")) {
        dispatcher.enqueue<QuitEvent>();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Reset UI")) {
        dispatcher.enqueue<RestoreLayoutEvent>();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Debug")) {
      if (ImGui::MenuItem("Open Style Editor...")) {
        dispatcher.enqueue<ShowStyleEditorEvent>();
      }

      if (ImGui::MenuItem("Open Demo Window...")) {
        dispatcher.enqueue<ShowDemoWindowEvent>();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

}  // namespace gravel
