#pragma once

#include "common/type/ecs.hpp"
#include "common/type/string.hpp"

namespace gravel {

class Scene final {
 public:
  auto make_node(String name, Entity parent = kNullEntity) -> Entity;
  auto make_node(Entity parent = kNullEntity) -> Entity;

  [[nodiscard]] auto get_registry() -> Registry& { return mRegistry; }
  [[nodiscard]] auto get_registry() const -> const Registry& { return mRegistry; }

 private:
  Registry mRegistry;
};

}  // namespace gravel
