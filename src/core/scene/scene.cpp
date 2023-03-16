#include "scene.hpp"

#include <utility>  // move

#include <fmt/format.h>

#include "common/debug/assert.hpp"
#include "common/primitives.hpp"
#include "scene/identifier.hpp"
#include "scene/node.hpp"
#include "scene/transform.hpp"

namespace gravel {

auto Scene::make_node(String name, const Entity parent) -> Entity
{
  const auto node_entity = mRegistry.create();

  mRegistry.emplace<Identifier>(node_entity, std::move(name));
  mRegistry.emplace<Transform>(node_entity);

  auto& node = mRegistry.emplace<Node>(node_entity);
  node.parent = parent;

  if (node.parent != entt::null) {
    mRegistry.emplace<ChildNode>(node_entity);

    auto& parent_node = mRegistry.get<Node>(node.parent);
    parent_node.children.push_back(node_entity);
  }

  GRAVEL_ASSERT(mRegistry.all_of<Identifier>(node_entity));
  GRAVEL_ASSERT(mRegistry.all_of<Transform>(node_entity));

  return node_entity;
}

auto Scene::make_node(const Entity parent) -> Entity
{
  const auto node_entity = make_node(String {}, parent);

  auto& name = mRegistry.get<Identifier>(node_entity);
  name.name = fmt::format("Node {}", static_cast<uint32>(node_entity));

  return node_entity;
}

}  // namespace gravel
