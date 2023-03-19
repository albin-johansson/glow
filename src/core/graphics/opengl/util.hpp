#pragma once

#include <cstdlib>  // exit, EXIT_FAILURE

#include "common/predef.hpp"
#include "common/primitives.hpp"
#include "common/type/string.hpp"
#include "graphics/buffer_usage.hpp"
#include "graphics/shader_type.hpp"

namespace gravel::gl {

auto check_errors() -> bool;
auto check_errors(const char* file, int line) -> bool;

void set_option(uint option, bool value);

[[nodiscard]] auto convert_shader_type(ShaderType type) -> uint;
[[nodiscard]] auto convert_buffer_usage(BufferUsage usage) -> uint;

[[nodiscard]] auto get_bound_vertex_array() -> uint;
[[nodiscard]] auto get_bound_vertex_buffer() -> uint;
[[nodiscard]] auto get_bound_index_buffer() -> uint;
[[nodiscard]] auto get_bound_uniform_buffer() -> uint;
[[nodiscard]] auto get_bound_framebuffer() -> uint;
[[nodiscard]] auto get_bound_texture() -> uint;
[[nodiscard]] auto get_bound_program() -> uint;

[[nodiscard]] auto get_renderer_name() -> String;
[[nodiscard]] auto get_vendor_name() -> String;
[[nodiscard]] auto get_version() -> String;
[[nodiscard]] auto get_glsl_version() -> String;

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
