#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_C(Scene);

class Backend {
 public:
  virtual ~Backend() = default;

  virtual void on_init([[maybe_unused]] Scene& scene) {}

  virtual void on_update(float32 dt) = 0;

  virtual void on_render(Scene& scene) = 0;

  [[nodiscard]] virtual auto should_quit() const -> bool = 0;
};

}  // namespace gravel
