#pragma once

#include "common/predef.hpp"
#include "common/type/dispatcher.hpp"

namespace glow {

GRAVEL_FORWARD_DECLARE_C(Scene);

void show_menu_bar(const Scene& scene, Dispatcher& dispatcher);

}  // namespace glow
