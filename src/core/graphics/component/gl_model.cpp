#include "gl_model.hpp"

#include <cstdlib>  // offsetof

#include <glad/glad.h>

#include "common/type/vector.hpp"
#include "io/model_loader.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto create_opengl_material(Registry& registry,
                                          const MaterialData& material_data) -> Entity
{
  const auto material_entity = registry.create();
  auto& gl_material = registry.emplace<comp::OpenGLMaterial>(material_entity);

  if (material_data.diffuse_tex.has_value()) {
    // TODO
  }

  if (material_data.specular_tex.has_value()) {
    // TODO
  }

  gl_material.ambient = material_data.ambient;
  gl_material.diffuse = material_data.diffuse;
  gl_material.specular = material_data.specular;
  gl_material.emission = material_data.emission;

  return material_entity;
}

[[nodiscard]] auto create_opengl_mesh(const MeshData& mesh_data,
                                      const Entity material_entity) -> comp::OpenGLMesh
{
  comp::OpenGLMesh gl_mesh;
  gl_mesh.material = material_entity;

  gl_mesh.transform = mesh_data.transform;
  gl_mesh.vertex_count = static_cast<uint>(mesh_data.vertices.size());
  gl_mesh.index_count = static_cast<uint>(mesh_data.indices.size());

  gl_mesh.vao.bind();

  gl_mesh.vbo.bind();
  gl_mesh.vbo.upload_data(mesh_data.vertices.size() * sizeof(Vertex),
                          mesh_data.vertices.data());

  using IndexType = decltype(MeshData::indices)::value_type;
  gl_mesh.ebo.bind();
  gl_mesh.ebo.upload_data(mesh_data.indices.size() * sizeof(IndexType),
                          mesh_data.indices.data());

  gl_mesh.vao.init_attr(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
  gl_mesh.vao.init_attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
  gl_mesh.vao.init_attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tex_coords));

  gl::VertexArray::unbind();
  gl::VertexBuffer::unbind();
  gl::IndexBuffer::unbind();

  return gl_mesh;
}

}  // namespace

void assign_opengl_model(Registry& registry, const Entity entity, const Path& path)
{
  if (const auto model_data = load_model_data(path)) {
    auto& gl_model = registry.emplace<comp::OpenGLModel>(entity);

    HashMap<usize, Entity> material_entities;
    material_entities.reserve(model_data->materials.size());

    for (const auto& [material_id, material_data] : model_data->materials) {
      const auto material_entity = create_opengl_material(registry, material_data);
      material_entities[material_id] = material_entity;
    }

    gl_model.meshes.reserve(model_data->meshes.size());
    for (const auto& mesh_data : model_data->meshes) {
      const auto material_entity = material_entities.at(mesh_data.material_id);
      gl_model.meshes.push_back(create_opengl_mesh(mesh_data, material_entity));
    }
  }
}

}  // namespace gravel