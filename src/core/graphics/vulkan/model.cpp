#include "model.hpp"

#include <utility>  // move

#include <spdlog/spdlog.h>

#include "graphics/vulkan/image/image.hpp"
#include "graphics/vulkan/image/image_cache.hpp"
#include "graphics/vulkan/image/image_view.hpp"
#include "io/model_loader.hpp"
#include "scene/scene.hpp"

namespace gravel::vk {
namespace {

[[nodiscard]] auto load_image_if_missing(Maybe<Path> path, ImageCache& cache)
    -> VkImageView
{
  if (path.has_value() && !cache.images.contains(*path)) {
    if (auto image =
            load_image_2d(*path, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT)) {
      ImageView view {*image, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT};
      VkImageView result = view.get();

      cache.views.try_emplace(image->get(), std::move(view));
      cache.images.try_emplace(*path, std::move(*image));

      return result;
    }
    else {
      spdlog::error("[VK] Failed to load model texture");
    }
  }

  return VK_NULL_HANDLE;
}

[[nodiscard]] auto create_material(Scene& scene,
                                   const MaterialData& material_data,
                                   const Path& model_dir) -> Entity
{
  const auto material_entity = scene.get_registry().create();
  auto& material = scene.add<Material>(material_entity);

  auto& cache = scene.get<ImageCache>();

  if (VkImageView diffuse = load_image_if_missing(material_data.diffuse_tex, cache)) {
    material.diffuse_tex = diffuse;
  }

  if (VkImageView specular = load_image_if_missing(material_data.specular_tex, cache)) {
    material.specular_tex = specular;
  }

  material.ambient = material_data.ambient;
  material.diffuse = material_data.diffuse;
  material.specular = material_data.specular;
  material.emission = material_data.emission;

  return material_entity;
}

[[nodiscard]] auto create_mesh(const MeshData& mesh_data, const Entity material_entity)
    -> Mesh
{
  Mesh mesh;
  mesh.transform = mesh_data.transform;
  mesh.material = material_entity;
  mesh.index_count = u32_size(mesh_data.indices);

  mesh.vertex_buffer = Buffer::create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      mesh_data.vertices.data(),
                                      byte_size(mesh_data.vertices));
  mesh.index_buffer = Buffer::create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     mesh_data.indices.data(),
                                     byte_size(mesh_data.indices));

  return mesh;
}

}  // namespace

void assign_model(Scene& scene, const Entity entity, const Path& path)
{
  if (const auto model_data = load_model_data(path, GraphicsApi::Vulkan)) {
    auto& model = scene.add<Model>(entity);
    model.meshes.reserve(model_data->meshes.size());

    HashMap<usize, Entity> material_entities;
    material_entities.reserve(model_data->materials.size());

    for (const auto& [material_id, material_data] : model_data->materials) {
      const auto material_entity = create_material(scene, material_data, model_data->dir);
      material_entities[material_id] = material_entity;
    }

    for (const auto& mesh_data : model_data->meshes) {
      const auto material_entity = material_entities.at(mesh_data.material_id);
      model.meshes.push_back(create_mesh(mesh_data, material_entity));
    }
  }
}

}  // namespace gravel::vk
