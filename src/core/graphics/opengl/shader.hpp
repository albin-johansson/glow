#pragma once

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/result.hpp"
#include "common/type/path.hpp"
#include "graphics/shader_type.hpp"

namespace glow::gl {

/**
 * Represents an OpenGL shader object.
 *
 * \details Instances of this class are usually short-lived, and can be safely
 *          discarded after being attached to a program instance.
 */
class Shader final {
 public:
  GLOW_DELETE_COPY(Shader);

  /// Creates a shader of the specified type, without any associated code.
  explicit Shader(ShaderType type);

  ~Shader() noexcept;

  Shader(Shader&& other) noexcept;

  auto operator=(Shader&& other) noexcept -> Shader&;

  /// Tries to compile the provided GLSL source code.
  auto compile_code(const char* code) -> Result;

  /// Tries to compile the GLSL shader file at the specified file path.
  auto compile_file(const Path& path) -> Result;

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace glow::gl
