#pragma once

#include "common/primitives.hpp"
#include "common/type/map.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"
#include "common/type/string.hpp"
#include "common/type/vector.hpp"
#include "graphics/graphics_api.hpp"
#include "graphics/vertex.hpp"

namespace glow {

struct MaterialData final {
  String name;

  // Texture paths
  Maybe<Path> diffuse_tex;
  Maybe<Path> specular_tex;
  Maybe<Path> ao_tex;
  Maybe<Path> metalness_tex;
  Maybe<Path> roughness_tex;
  Maybe<Path> normal_tex;

  // Base colors
  Vec3 ambient {};
  Vec3 diffuse {};
  Vec3 specular {};
  Vec3 emission {};

  // Factor attributes, all within [0, 1].
  float roughness {};  ///< Perfectly smooth <-> completely rough.
  float metalness {};  ///< Dielectric <-> metal.
  float sheen {};

  float shininess {};
  float refraction_index {};
};

struct MeshData final {
  Mat4 transform {1.0f};
  Vector<Vertex> vertices;
  Vector<uint32> indices;
  usize material_id {};
};

struct ModelData final {
  Path dir;
  Vector<MeshData> meshes;
  HashMap<usize, MaterialData> materials;
};

/// Attempts to load a 3D model file.
///
/// \details
/// The specified graphics API is used to determine how to adjust the model data,
/// e.g. OpenGL and Vulkan uses different coordinate systems and texture coordinates,
/// which this function will take care of.
///
/// \param path file path to the model file.
/// \param api the graphics API that will be used to render the model.
[[nodiscard]] auto load_model_data(const Path& path, GraphicsAPI api) -> Maybe<ModelData>;

}  // namespace glow
