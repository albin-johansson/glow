#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/math.hpp"

namespace gravel::gl {

GRAVEL_FORWARD_DECLARE_S(Material);

/// This struct corresponds to a std140 layout uniform block.
struct EnvironmentBuffer final {
  Mat4 inverse_proj_view {};
  Vec4 camera_pos {};
  float32 brightness {1.0f};
  float32 gamma {2.2f};
  int32 use_gamma_correction {true};
};

/// This struct corresponds to a std140 layout uniform block.
struct MatrixBuffer final {
  Mat4 m {};       ///< Model matrix.
  Mat4 mv {};      ///< Model-view matrix.
  Mat4 mvp {};     ///< Model-view-projection matrix.
  Mat4 normal {};  ///< Normal matrix.

  void update(const Mat4& model, const Mat4& view, const Mat4& projection);
};

/// This struct corresponds to a std140 layout uniform block.
struct MaterialBuffer final {
  Vec4 ambient {};
  Vec4 diffuse {};
  Vec4 specular {};
  Vec4 emission {};
  int32 has_ambient_tex {false};
  int32 has_diffuse_tex {false};
  int32 has_specular_tex {false};
  int32 has_emission_tex {false};

  void update(const Material& material);
};

}  // namespace gravel::gl