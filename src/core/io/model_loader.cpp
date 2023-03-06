#include "model_loader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION

#include <chrono>  // system_clock, duration_cast, milliseconds

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>

#include "common/primitives.hpp"
#include "common/type/map.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"

namespace gravel {

namespace chrono = std::chrono;

auto load_obj_model(const Path& path) -> Maybe<ModelData>
{
  const auto start_time = chrono::system_clock::now();

  if (path.extension() != ".obj") {
    spdlog::error("[IO] OBJ model files should have '.obj' extension");
    return kNothing;
  }

  const auto absolute_path = fs::absolute(path).string();
  const auto directory = path.parent_path().string();

  tinyobj::attrib_t attrib {};
  Vector<tinyobj::shape_t> shapes;
  Vector<tinyobj::material_t> materials;

  String warn;
  String err;

  const bool triangulate = true;
  const bool fallback_vertex_colors = false;
  const bool success = tinyobj::LoadObj(&attrib,
                                        &shapes,
                                        &materials,
                                        &warn,
                                        &err,
                                        absolute_path.c_str(),
                                        directory.c_str(),
                                        triangulate,
                                        fallback_vertex_colors);

  if (!warn.empty()) {
    spdlog::warn("[IO] {}", warn);
  }

  if (!err.empty()) {
    spdlog::error("[IO] {}", err);
  }

  if (!success) {
    return kNothing;
  }

  if (attrib.vertices.size() != attrib.normals.size()) {
    spdlog::error("[IO] Vertex count must match normal count");
    return kNothing;
  }

  ModelData model;
  model.vertices.reserve(attrib.vertices.size());
  // model.materials.reserve(materials.size());
  // model.meshes.reserve(shapes.size());

  HashMap<Vertex, uint> vertex_indices;

  model.indices.reserve(attrib.vertices.size());
  vertex_indices.reserve(attrib.vertices.size() / 4);

  // TODO handle meshes and materials
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex;

      const auto position_index = 3 * static_cast<usize>(index.vertex_index);
      const auto normal_index = 3 * static_cast<usize>(index.normal_index);
      const auto tex_coords_index = 2 * static_cast<usize>(index.texcoord_index);

      vertex.position = Vec3 {
          attrib.vertices.at(position_index + 0),
          attrib.vertices.at(position_index + 1),
          attrib.vertices.at(position_index + 2),
      };

      vertex.normal = Vec3 {
          attrib.normals.at(normal_index + 0),
          attrib.normals.at(normal_index + 1),
          attrib.normals.at(normal_index + 2),
      };

      vertex.tex_coords = Vec2 {
          attrib.texcoords.at(tex_coords_index + 0),
          attrib.texcoords.at(tex_coords_index + 1),
      };

      auto [iter, did_insert] = vertex_indices.try_emplace(vertex);
      if (did_insert) {
        const auto vertex_index = static_cast<uint>(model.vertices.size());
        iter->second = vertex_index;
        model.vertices.push_back(vertex);
      }

      model.indices.push_back(vertex_indices.at(vertex));
    }
  }

  const auto end_time = chrono::system_clock::now();
  const auto total_duration =
      chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

  return model;
}

}  // namespace gravel
