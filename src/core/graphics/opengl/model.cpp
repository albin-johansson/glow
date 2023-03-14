#include "model.hpp"

#include <cstdlib>  // offsetof

#include <glad/glad.h>

#include "common/type/vector.hpp"
#include "io/model_loader.hpp"

namespace gravel::gl {
namespace {

[[nodiscard]] auto create_material(Registry& registry, const MaterialData& material_data)
    -> Entity
{
  const auto material_entity = registry.create();
  auto& material = registry.emplace<Material>(material_entity);

  if (material_data.diffuse_tex.has_value()) {
    // TODO
  }

  if (material_data.specular_tex.has_value()) {
    // TODO
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
  mesh.index_count = static_cast<uint>(mesh_data.indices.size());

  mesh.vao.bind();

  mesh.vbo.bind();
  mesh.vbo.upload_data(mesh_data.vertices.size() * sizeof(Vertex),
                       mesh_data.vertices.data());

  using IndexType = decltype(MeshData::indices)::value_type;
  mesh.ebo.bind();
  mesh.ebo.upload_data(mesh_data.indices.size() * sizeof(IndexType),
                       mesh_data.indices.data());

  mesh.vao.init_attr(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
  mesh.vao.init_attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
  mesh.vao.init_attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tex_coords));

  gl::VertexArray::unbind();
  gl::VertexBuffer::unbind();
  gl::IndexBuffer::unbind();

  return mesh;
}

}  // namespace

void assign_model(Registry& registry, const Entity entity, const Path& path)
{
  if (const auto model_data = load_model_data(path)) {
    auto& model = registry.emplace<Model>(entity);
    model.meshes.reserve(model_data->meshes.size());

    HashMap<usize, Entity> material_entities;
    material_entities.reserve(model_data->materials.size());

    for (const auto& [material_id, material_data] : model_data->materials) {
      const auto material_entity = create_material(registry, material_data);
      material_entities[material_id] = material_entity;
    }

    for (const auto& mesh_data : model_data->meshes) {
      const auto material_entity = material_entities.at(mesh_data.material_id);
      model.meshes.push_back(create_mesh(mesh_data, material_entity));
    }
  }
}

}  // namespace gravel::gl