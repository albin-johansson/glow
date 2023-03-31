#include "model.hpp"

#include "io/model_loader.hpp"
#include "scene/scene.hpp"

namespace gravel::vlk {
namespace {

[[nodiscard]] auto create_material(Scene& scene,
                                   const MaterialData& material_data,
                                   const Path& model_dir) -> Entity
{
  const auto material_entity = scene.get_registry().create();
  auto& material = scene.add<Material>(material_entity);

  if (material_data.diffuse_tex.has_value()) {
    // TODO load diffuse texture
  }

  if (material_data.specular_tex.has_value()) {
    // TODO load specular texture
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
  mesh.index_count = static_cast<uint32>(mesh_data.indices.size());

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

}  // namespace gravel::vlk
