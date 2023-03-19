#include "scene.hpp"

#include <utility>  // move

#include <fmt/format.h>

#include "common/debug/assert.hpp"
#include "common/primitives.hpp"
#include "graphics/camera.hpp"
#include "graphics/environment.hpp"
#include "graphics/renderer_info.hpp"
#include "graphics/rendering_options.hpp"
#include "scene/identifier.hpp"
#include "scene/node.hpp"
#include "scene/transform.hpp"
#include "ui/camera_options.hpp"
#include "ui/gizmos.hpp"

namespace gravel {

Scene::Scene()
{
  auto& ctx = mRegistry.ctx();
  ctx.emplace<CameraContext>();
  ctx.emplace<CameraOptions>();
  ctx.emplace<EnvironmentOptions>();
  ctx.emplace<GizmosOptions>();
  ctx.emplace<RendererInfo>();

  auto& rendering_options = ctx.emplace<RenderingOptions>();
  rendering_options.options[RenderingOption::VSync];
  rendering_options.options[RenderingOption::DepthTest];
  rendering_options.options[RenderingOption::FaceCulling];
  rendering_options.options[RenderingOption::Wireframe];
  rendering_options.options[RenderingOption::Blending];
}

auto Scene::make_node(String name, const Entity parent) -> Entity
{
  const auto node_entity = mRegistry.create();

  mRegistry.emplace<Identifier>(node_entity, std::move(name));
  mRegistry.emplace<Transform>(node_entity);
  mRegistry.emplace<TransformOptions>(node_entity);

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

auto Scene::has_active_camera() const -> bool
{
  return get<CameraContext>().active_camera != kNullEntity;
}

auto Scene::get_active_camera() -> Pair<Entity, Camera&>
{
  const auto& camera_context = get<CameraContext>();
  auto& camera = get<Camera>(camera_context.active_camera);

  return {camera_context.active_camera, camera};
}

auto Scene::get_active_camera() const -> Pair<Entity, const Camera&>
{
  const auto& camera_context = get<CameraContext>();
  const auto& camera = get<Camera>(camera_context.active_camera);

  return {camera_context.active_camera, camera};
}

}  // namespace gravel
