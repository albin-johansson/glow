#include "dock_space.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace gravel {

void update_dock_space(const bool restore_layout)
{
#if 0
  const auto root_dock_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

  if (restore_layout) {
    const auto size = ImGui::GetMainViewport()->Size;
    if (size.x > 0 && size.y > 0) {
      ImGui::DockBuilderRemoveNodeChildNodes(root_dock_id);

      auto id = root_dock_id;
      auto right = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.33f, nullptr, &id);
      // auto right_bottom =
      //     ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.5f, nullptr, &right);

      ImGui::DockBuilderDockWindow("Viewport", id);
      ImGui::DockBuilderDockWindow("Scene", right);

      ImGui::DockBuilderFinish(id);
    }
  }
#endif
}

}  // namespace gravel
