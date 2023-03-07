#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

namespace gravel {

using Entity = entt::entity;
using Registry = entt::registry;

inline constexpr entt::entity kNullEntity {entt::null};

}  // namespace gravel
