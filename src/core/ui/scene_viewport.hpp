#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"

namespace glow {

GLOW_FORWARD_DECLARE_C(Scene);
GLOW_FORWARD_DECLARE_C(Backend);

void show_scene_viewport(const Scene& scene, Backend& backend, Dispatcher& dispatcher);

}  // namespace glow
