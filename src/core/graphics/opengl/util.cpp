#include "util.hpp"

#include <unordered_map>  // unordered_map

#include <fmt/format.h>
#include <glad/glad.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/debug/error.hpp"

namespace gravel::gl {
namespace {

inline const std::unordered_map<uint, const char*> kErrorMessages = {
    {GL_NO_ERROR, "GL_NO_ERROR"},
    {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
    {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
    {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"},
    {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    {GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW"},
    {GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW"},
};

[[nodiscard]] auto get_integer(const uint name) -> uint
{
  int id {};
  glGetIntegerv(name, &id);
  return static_cast<uint>(id);
}

}  // namespace

auto check_errors() -> bool
{
  bool had_error = false;

  for (uint err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
    had_error = true;
    spdlog::error("[GL] Error: {}", kErrorMessages.at(err));
  }

  return had_error;
}

auto check_errors(const char* file, const int line) -> bool
{
  bool had_error = false;

  for (uint err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
    had_error = true;
    spdlog::error("[GL] {}:{} Error: {}", file, line, kErrorMessages.at(err));
  }

  return had_error;
}

auto convert_shader_type(const ShaderType type) -> uint
{
  switch (type) {
    case ShaderType::Vertex:
      return GL_VERTEX_SHADER;

    case ShaderType::Fragment:
      return GL_FRAGMENT_SHADER;

    default:
      throw Error {fmt::format("[GL] Unsupported shader type: {}",  //
                               magic_enum::enum_name(type))};
  }
}

auto convert_buffer_usage(const BufferUsage usage) -> uint
{
  switch (usage) {
    case BufferUsage::Static:
      return GL_STATIC_DRAW;

    case BufferUsage::Dynamic:
      return GL_DYNAMIC_DRAW;

    case BufferUsage::Stream:
      return GL_STREAM_DRAW;

    default:
      throw Error {fmt::format("[GL] Unsupported buffer usage: {}",  //
                               magic_enum::enum_name(usage))};
  }
}

auto get_bound_vertex_array() -> uint
{
  return get_integer(GL_VERTEX_ARRAY_BINDING);
}

auto get_bound_vertex_buffer() -> uint
{
  return get_integer(GL_ARRAY_BUFFER_BINDING);
}

auto get_bound_index_buffer() -> uint
{
  return get_integer(GL_ELEMENT_ARRAY_BUFFER_BINDING);
}

auto get_bound_uniform_buffer() -> uint
{
  return get_integer(GL_UNIFORM_BUFFER_BINDING);
}

auto get_bound_framebuffer() -> uint
{
  return get_integer(GL_FRAMEBUFFER_BINDING);
}

auto get_bound_texture() -> uint
{
  return get_integer(GL_TEXTURE_BINDING_2D);
}

}  // namespace gravel::gl
