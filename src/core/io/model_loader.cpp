#include "model_loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include "common/type/chrono.hpp"
#include "common/type/map.hpp"

namespace gravel {
namespace {

[[nodiscard]] auto convert_matrix(const aiMatrix4x4& row_major) -> Mat4
{
  Mat4 column_major {1.0f};

  column_major[0].x = row_major.a1;
  column_major[0].y = row_major.b1;
  column_major[0].z = row_major.c1;
  column_major[0].w = row_major.d1;

  column_major[1].x = row_major.a2;
  column_major[1].y = row_major.b2;
  column_major[1].z = row_major.c2;
  column_major[1].w = row_major.d2;

  column_major[2].x = row_major.a3;
  column_major[2].y = row_major.b3;
  column_major[2].z = row_major.c3;
  column_major[2].w = row_major.d3;

  column_major[3].x = row_major.a4;
  column_major[3].y = row_major.b4;
  column_major[3].z = row_major.c4;
  column_major[3].w = row_major.d4;

  return column_major;
}

[[nodiscard]] auto get_texture_path(const aiMaterial* material, const aiTextureType type)
    -> Maybe<Path>
{
  if (material->GetTextureCount(type) > 0) {
    aiString path;
    material->GetTexture(type, 0, &path);
    return Path {path.C_Str()};
  }
  else {
    return kNothing;
  }
}

[[nodiscard]] auto load_material_data(const aiScene* scene, const aiMesh* mesh)
    -> MaterialData
{
  auto* material = scene->mMaterials[mesh->mMaterialIndex];

  MaterialData material_data;
  material_data.diffuse_tex = get_texture_path(material, aiTextureType_DIFFUSE);
  material_data.specular_tex = get_texture_path(material, aiTextureType_SPECULAR);

  return material_data;
}

[[nodiscard]] auto create_mesh_vertex(const aiMesh* mesh, const uint vertex_idx) -> Vertex
{
  Vertex vertex;

  const auto& position = mesh->mVertices[vertex_idx];
  vertex.position.x = position.x;
  vertex.position.y = position.y;
  vertex.position.z = position.z;

  if (mesh->HasNormals()) {
    const auto& normal = mesh->mNormals[vertex_idx];
    vertex.normal.x = normal.x;
    vertex.normal.y = normal.y;
    vertex.normal.z = normal.z;
  }

  if (mesh->HasTextureCoords(0)) {
    const auto& tex_coords = mesh->mTextureCoords[0][vertex_idx];
    vertex.tex_coords.x = tex_coords.x;
    vertex.tex_coords.y = tex_coords.y;
  }
  else {
    vertex.tex_coords = Vec2 {0, 0};
  }

  return vertex;
}

void process_mesh(ModelData& model,
                  const aiScene* scene,
                  const aiNode* node,
                  const aiMesh* mesh)
{
  auto& mesh_data = model.meshes.emplace_back();
  mesh_data.transform = convert_matrix(node->mTransformation);

  for (uint vertex_idx = 0; vertex_idx < mesh->mNumVertices; ++vertex_idx) {
    mesh_data.vertices.push_back(create_mesh_vertex(mesh, vertex_idx));
  }

  for (uint face_idx = 0; face_idx < mesh->mNumFaces; ++face_idx) {
    const auto& face = mesh->mFaces[face_idx];

    for (uint index_idx = 0; index_idx < face.mNumIndices; ++index_idx) {
      mesh_data.indices.push_back(face.mIndices[index_idx]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    model.materials.push_back(load_material_data(scene, mesh));
  }
}

void process_node(ModelData& model, const aiScene* scene, const aiNode* node)
{
  for (uint mesh_idx = 0; mesh_idx < node->mNumMeshes; ++mesh_idx) {
    process_mesh(model, scene, node, scene->mMeshes[node->mMeshes[mesh_idx]]);
  }

  for (uint child_idx = 0; child_idx < node->mNumChildren; ++child_idx) {
    process_node(model, scene, node->mChildren[child_idx]);
  }
}

}  // namespace

auto load_model_data(const Path& path) -> Maybe<ModelData>
{
  const auto start_time = Clock::now();

  const auto absolute_path = fs::absolute(path).string();
  const auto directory = path.parent_path().string();

  Assimp::Importer importer;
  const auto* scene =
      importer.ReadFile(path.string(), aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    spdlog::error("[IO] Could not read 3D object file: {}", importer.GetErrorString());
    return kNothing;
  }

  ModelData model;
  process_node(model, scene, scene->mRootNode);

  const auto end_time = Clock::now();
  const auto total_duration = chrono::duration_cast<Milliseconds>(end_time - start_time);
  spdlog::debug("[IO] Loaded 3D model in {} (meshes: {}, materials: {})",
                total_duration,
                model.meshes.size(),
                model.materials.size());

  return model;
}

}  // namespace gravel
