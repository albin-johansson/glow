#pragma once

#include <cstdlib>  // exit, EXIT_FAILURE

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "graphics/buffer_usage.hpp"
#include "graphics/shader_type.hpp"

namespace gravel::gl {

auto check_errors() -> bool;
auto check_errors(const char* file, int line) -> bool;

[[nodiscard]] auto convert_shader_type(ShaderType type) -> uint;
[[nodiscard]] auto convert_buffer_usage(BufferUsage usage) -> uint;

[[nodiscard]] auto get_bound_vertex_array() -> uint;
[[nodiscard]] auto get_bound_vertex_buffer() -> uint;
[[nodiscard]] auto get_bound_index_buffer() -> uint;
[[nodiscard]] auto get_bound_uniform_buffer() -> uint;
[[nodiscard]] auto get_bound_framebuffer() -> uint;
[[nodiscard]] auto get_bound_texture() -> uint;

}  // namespace gravel::gl

#if GRAVEL_DEBUG_BUILD
#define GRAVEL_GL_CHECK_ERRORS()                        \
  do {                                                  \
    if (gravel::gl::check_errors(__FILE__, __LINE__)) { \
      GRAVEL_DEBUG_BREAK;                               \
      std::exit(EXIT_FAILURE);                          \
    }                                                   \
  } while (false)
#else
#define GRAVEL_GL_CHECK_ERRORS() \
  do {                           \
  } while (false)
#endif  // GRAVEL_DEBUG_BUILD