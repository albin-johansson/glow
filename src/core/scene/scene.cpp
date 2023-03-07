#include "scene.hpp"

#include <cassert>  // assert
#include <utility>  // move

#include <fmt/format.h>

#include "common/primitives.hpp"
#include "scene/component/identifier.hpp"
#include "scene/component/node.hpp"
#include "scene/component/transform.hpp"

namespace gravel {

auto Scene::make_node(String name, const Entity parent) -> Entity
{
  const auto node_entity = mRegistry.create();

  mRegistry.emplace<comp::Identifier>(node_entity, std::move(name));
  mRegistry.emplace<comp::Transform>(node_entity);

  auto& node = mRegistry.emplace<comp::Node>(node_entity);
  node.parent = parent;

  if (node.parent != entt::null) {
    mRegistry.emplace<comp::ChildNode>(node_entity);

    auto& parent_node = mRegistry.get<comp::Node>(node.parent);
    parent_node.children.push_back(node_entity);
  }

  assert(mRegistry.all_of<comp::Identifier>(node_entity));
  assert(mRegistry.all_of<comp::Transform>(node_entity));

  return node_entity;
}

auto Scene::make_node(const Entity parent) -> Entity
{
  const auto node_entity = make_node(String {}, parent);

  auto& name = mRegistry.get<comp::Identifier>(node_entity);
  name.name = fmt::format("Node {}", static_cast<uint32>(node_entity));

  return node_entity;
}

}  // namespace gravel
