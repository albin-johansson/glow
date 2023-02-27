#pragma once

#include <utility>  // forward

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/result.hpp"
#include "common/type/math.hpp"
#include "common/type/maybe.hpp"
#include "common/type/path.hpp"

namespace gravel::gl {

GRAVEL_FORWARD_DECLARE(Shader);

/// Represents an OpenGL shader program.
class Program final {
 public:
  GRAVEL_DELETE_COPY(Program);

  /// Creates a shader program without any attached shaders.
  Program();

  ~Program() noexcept;

  Program(Program&& other) noexcept;

  auto operator=(Program&& other) noexcept -> Program&;

  /// Tries to load a vertex/fragment shader pair from the specified file paths.
  auto load_shader_files(const Path& vertex_path, const Path& fragment_path) -> Result;

  /// Tries to compile a vertex/fragment shader pair from the provided shader source code.
  auto load_shader_code(const char* vertex_code, const char* fragment_code) -> Result;

  /// Attaches a shader to the program.
  void attach_shader(const Shader& shader);

  /// Attempts to link the shader program, using previously attached shaders.
  auto link() -> Result;

  /// Enables the associated shaders for subsequent draw calls.
  void bind();

  /// Disables any bound shader program.
  static void unbind();

  /// Sets the binding index of a uniform buffer block (similar to sampler bindings).
  auto set_uniform_block_binding(const char* name, int binding) -> Result;

  void set_uniform(int location, bool b);

  void set_uniform(int location, int i);

  void set_uniform(int location, float f);

  void set_uniform(int location, const Vec2& vec);

  void set_uniform(int location, const Vec2i& vec);

  void set_uniform(int location, const Vec3& vec);

  void set_uniform(int location, const Vec3i& vec);

  void set_uniform(int location, const Vec4& vec);

  void set_uniform(int location, const Vec4i& vec);

  void set_uniform(int location, const Mat3& mat);

  void set_uniform(int location, const Mat4& mat);

  template <typename T>
  auto set_uniform(const char* name, T&& value) -> Result
  {
    if (const auto location = get_uniform_location(name)) {
      set_uniform(*location, std::forward<T>(value));
      return kSuccess;
    }
    else {
      return kFailure;
    }
  }

  void set_uniform_array(int location, ssize count, const int* array);

  void set_uniform_array(int location, ssize count, const float* array);

  void set_uniform_array(int location, ssize count, const Vec2* array);

  void set_uniform_array(int location, ssize count, const Vec2i* array);

  void set_uniform_array(int location, ssize count, const Vec3* array);

  void set_uniform_array(int location, ssize count, const Vec3i* array);

  void set_uniform_array(int location, ssize count, const Vec4* array);

  void set_uniform_array(int location, ssize count, const Vec4i* array);

  template <typename T>
  auto set_uniform_array(const char* name, const ssize count, const T* array) -> Result
  {
    if (const auto location = get_uniform_location(name)) {
      set_uniform_array(*location, count, array);
      return kSuccess;
    }
    else {
      return kFailure;
    }
  }

  /// Returns the location of a uniform in the program.
  [[nodiscard]] auto get_uniform_location(const char* name) const -> Maybe<int>;

  [[nodiscard]] auto get_uniform_block_index(const char* name) const -> Maybe<int>;

  [[nodiscard]] auto get_uniform_block_size(int block_index) const -> Maybe<int>;

  [[nodiscard]] auto get_id() const -> uint { return mID; }

 private:
  uint mID {};

  void dispose() noexcept;
};

}  // namespace gravel::gl
