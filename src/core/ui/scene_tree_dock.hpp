#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"

namespace glow {

GLOW_FORWARD_DECLARE_C(Scene);

void show_scene_tree_dock(const Scene& scene, Dispatcher& dispatcher);

}  // namespace glow
