#include "model.hpp"

#include <cstddef>  // offsetof
#include <utility>  // move

#include <glad/glad.h>

#include "common/type/map.hpp"
#include "common/type/vector.hpp"
#include "graphics/opengl/texture_cache.hpp"
#include "io/model_loader.hpp"
#include "io/texture_loader.hpp"
#include "scene/scene.hpp"

namespace glow::gl {
namespace {

[[nodiscard]] auto _create_texture(TextureCache& cache, Path path) -> Maybe<uint>
{
  if (const auto iter = cache.textures.find(path); iter != cache.textures.end()) {
    return iter->second.get_id();
  }

  if (auto texture = Texture2D::load_rgb(path)) {
    texture->bind();
    texture->set_anisotropic_filtering(true);
    texture->generate_mipmap();
    Texture2D::unbind();

    const auto texture_id = texture->get_id();
    cache.textures.try_emplace(std::move(path), std::move(*texture));
    return texture_id;
  }

  return kNothing;
}

[[nodiscard]] auto _create_material(Scene& scene,
                                    const MaterialData& material_data,
                                    const Path& model_dir) -> Entity
{
  auto& texture_cache = scene.get<TextureCache>();

  const auto material_entity = scene.get_registry().create();
  auto& material = scene.add<Material>(material_entity);

  if (material_data.diffuse_tex.has_value()) {
    material.diffuse_tex =
        _create_texture(texture_cache, model_dir / *material_data.diffuse_tex);
  }

  if (material_data.specular_tex.has_value()) {
    material.specular_tex =
        _create_texture(texture_cache, model_dir / *material_data.specular_tex);
  }

  material.ambient = material_data.ambient;
  material.diffuse = material_data.diffuse;
  material.specular = material_data.specular;
  material.emission = material_data.emission;

  return material_entity;
}

[[nodiscard]] auto _create_mesh(const MeshData& mesh_data, const Entity material_entity)
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

  VertexArray::unbind();
  VertexBuffer::unbind();
  IndexBuffer::unbind();

  return mesh;
}

}  // namespace

void assign_model(Scene& scene, const Entity entity, const Path& path)
{
  if (const auto model_data = load_model_data(path, GraphicsAPI::OpenGL)) {
    auto& model = scene.add<Model>(entity);
    model.meshes.reserve(model_data->meshes.size());

    HashMap<usize, Entity> material_entities;
    material_entities.reserve(model_data->materials.size());

    for (const auto& [material_id, material_data] : model_data->materials) {
      const auto material_entity =
          _create_material(scene, material_data, model_data->dir);
      material_entities[material_id] = material_entity;
    }

    for (const auto& mesh_data : model_data->meshes) {
      const auto material_entity = material_entities.at(mesh_data.material_id);
      model.meshes.push_back(_create_mesh(mesh_data, material_entity));
    }
  }
}

}  // namespace glow::gl