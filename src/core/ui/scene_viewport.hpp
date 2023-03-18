#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_C(Scene);
GRAVEL_FORWARD_DECLARE_C(Backend);

void show_scene_viewport(const Scene& scene, Backend& backend, Dispatcher& dispatcher);

}  // namespace gravel
