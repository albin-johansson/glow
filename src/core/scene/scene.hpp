#pragma once

#include "common/predef.hpp"
#include "common/type/ecs.hpp"
#include "common/type/pair.hpp"
#include "common/type/string.hpp"

namespace gravel {

GRAVEL_FORWARD_DECLARE_S(Camera);

class Scene final {
 public:
  Scene();

  auto make_node(String name, Entity parent = kNullEntity) -> Entity;

  auto make_node(Entity parent = kNullEntity) -> Entity;

  template <typename T>
  auto add() -> T&
  {
    return mRegistry.ctx().emplace<T>();
  }

  template <typename T>
  auto add(const Entity entity) -> T&
  {
    return mRegistry.emplace<T>(entity);
  }

  template <typename T>
  [[nodiscard]] auto get(const Entity entity) -> T&
  {
    return mRegistry.get<T>(entity);
  }

  template <typename T>
  [[nodiscard]] auto get(const Entity entity) const -> const T&
  {
    return mRegistry.get<T>(entity);
  }

  template <typename T>
  [[nodiscard]] auto find(const Entity entity) const -> const T*
  {
    return mRegistry.try_get<T>(entity);
  }

  template <typename T>
  [[nodiscard]] auto get() -> T&
  {
    return mRegistry.ctx().get<T>();
  }

  template <typename T>
  [[nodiscard]] auto get() const -> const T&
  {
    return mRegistry.ctx().get<T>();
  }

  template <typename... Args>
  [[nodiscard]] auto each() const
  {
    return mRegistry.view<Args...>().each();
  }

  [[nodiscard]] auto has_active_camera() const -> bool;

  [[nodiscard]] auto get_active_camera() -> Pair<Entity, Camera&>;
  [[nodiscard]] auto get_active_camera() const -> Pair<Entity, const Camera&>;

  [[nodiscard]] auto get_registry() -> Registry& { return mRegistry; }
  [[nodiscard]] auto get_registry() const -> const Registry& { return mRegistry; }

 private:
  Registry mRegistry;
};

}  // namespace gravel
