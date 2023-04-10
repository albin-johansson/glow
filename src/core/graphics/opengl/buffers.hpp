#pragma once

#include "common/primitives.hpp"
#include "common/type/math.hpp"

namespace gravel::gl {

/// This struct corresponds to a std140 layout uniform block.
struct EnvironmentBuffer final {
  alignas(16) Mat4 inverse_proj_view {};
  alignas(16) Vec4 camera_pos {};
  float32 brightness {1.0f};
  float32 gamma {2.2f};
  int32 use_gamma_correction {true};
};

/// This struct corresponds to a std140 layout uniform block.
struct MatrixBuffer final {
  alignas(16) Mat4 m {};       ///< Model matrix.
  alignas(16) Mat4 mv {};      ///< Model-view matrix.
  alignas(16) Mat4 mvp {};     ///< Model-view-projection matrix.
  alignas(16) Mat4 normal {};  ///< Normal matrix.
};

/// This struct corresponds to a std140 layout uniform block.
struct MaterialBuffer final {
  alignas(16) Vec4 ambient {};
  alignas(16) Vec4 diffuse {};
  alignas(16) Vec4 specular {};
  alignas(16) Vec4 emission {};
  int32 has_ambient_tex {false};
  int32 has_diffuse_tex {false};
  int32 has_specular_tex {false};
  int32 has_emission_tex {false};
};

}  // namespace gravel::gl