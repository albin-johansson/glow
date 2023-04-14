#include "program.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include "graphics/opengl/shader.hpp"
#include "graphics/opengl/util.hpp"

namespace glow::gl {

Program::Program()
    : mID {glCreateProgram()}
{
}

Program::~Program() noexcept
{
  dispose();
}

Program::Program(Program&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto Program::operator=(Program&& other) noexcept -> Program&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void Program::dispose() noexcept
{
  if (mID != 0) {
    glDeleteProgram(mID);
  }
}

auto Program::load_shader_files(const Path& vertex_path, const Path& fragment_path)
    -> Result
{
  Shader vs {ShaderType::Vertex};
  if (vs.compile_file(vertex_path).failed()) {
    return kFailure;
  }

  Shader fs {ShaderType::Fragment};
  if (fs.compile_file(fragment_path).failed()) {
    return kFailure;
  }

  attach_shader(vs);
  attach_shader(fs);

  return kSuccess;
}

auto Program::load_shader_code(const char* vertex_code, const char* fragment_code)
    -> Result
{
  Shader vs {ShaderType::Vertex};
  if (vs.compile_code(vertex_code).failed()) {
    return kFailure;
  }

  Shader fs {ShaderType::Fragment};
  if (fs.compile_code(fragment_code).failed()) {
    return kFailure;
  }

  attach_shader(vs);
  attach_shader(fs);

  return kSuccess;
}

void Program::attach_shader(const Shader& shader)
{
  glAttachShader(mID, shader.get_id());
}

auto Program::link() -> Result
{
  glLinkProgram(mID);

  int success {};
  glGetProgramiv(mID, GL_LINK_STATUS, &success);

  if (success) {
    return kSuccess;
  }
  else {
    char msg[512] {};
    glGetProgramInfoLog(mID, sizeof msg, nullptr, msg);
    spdlog::error("[GL] Link error: {}", msg);
    return kFailure;
  }
}

void Program::bind()
{
  glUseProgram(mID);
}

void Program::unbind()
{
  glUseProgram(0);
}

auto Program::set_uniform_block_binding(const char* name, const int binding) -> Result
{
  if (const auto block_index = get_uniform_block_index(name)) {
    glUniformBlockBinding(mID, *block_index, binding);
    return kSuccess;
  }
  else {
    return kFailure;
  }
}

void Program::set_uniform(const int location, const bool b)
{
  glProgramUniform1i(mID, location, b ? GL_TRUE : GL_FALSE);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const int i)
{
  glProgramUniform1i(mID, location, i);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const float f)
{
  glProgramUniform1f(mID, location, f);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec2& vec)
{
  glProgramUniform2f(mID, location, vec.x, vec.y);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec2i& vec)
{
  glProgramUniform2i(mID, location, vec.x, vec.y);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec3& vec)
{
  glProgramUniform3f(mID, location, vec.x, vec.y, vec.z);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec3i& vec)
{
  glProgramUniform3i(mID, location, vec.x, vec.y, vec.z);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec4& vec)
{
  glProgramUniform4f(mID, location, vec.x, vec.y, vec.z, vec.w);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Vec4i& vec)
{
  glProgramUniform4i(mID, location, vec.x, vec.y, vec.z, vec.w);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Mat3& mat)
{
  glProgramUniformMatrix3fv(mID, location, 1, GL_FALSE, glm::value_ptr(mat));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform(const int location, const Mat4& mat)
{
  glProgramUniformMatrix4fv(mID, location, 1, GL_FALSE, glm::value_ptr(mat));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const int* array)
{
  glProgramUniform1iv(mID, location, static_cast<GLsizei>(count), array);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const float* array)
{
  glProgramUniform1fv(mID, location, static_cast<GLsizei>(count), array);
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec2* array)
{
  glProgramUniform2fv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec2i* array)
{
  glProgramUniform2iv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec3* array)
{
  glProgramUniform3fv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec3i* array)
{
  glProgramUniform3iv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec4* array)
{
  glProgramUniform4fv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

void Program::set_uniform_array(const int location, const ssize count, const Vec4i* array)
{
  glProgramUniform4iv(mID, location, static_cast<GLsizei>(count), glm::value_ptr(*array));
  GRAVEL_GL_CHECK_ERRORS();
}

auto Program::get_uniform_location(const char* name) const -> Maybe<int>
{
  const auto index = glGetUniformLocation(mID, name);
  if (index != -1) {
    return index;
  }
  else {
    return kNothing;
  }
}

auto Program::get_uniform_block_index(const char* name) const -> Maybe<int>
{
  const auto index = glGetUniformBlockIndex(mID, name);
  if (index != GL_INVALID_INDEX) {
    return index;
  }
  else {
    spdlog::error("[GL] Found no uniform block index for '{}'", name);
    return kNothing;
  }
}

auto Program::get_uniform_block_size(const int block_index) const -> Maybe<int>
{
  int block_size = -1;
  glGetActiveUniformBlockiv(mID, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
  GRAVEL_GL_CHECK_ERRORS();

  if (block_size != -1) {
    return block_size;
  }
  else {
    spdlog::error("[GL] Failed to get uniform block size for index '{}'", block_index);
    return kNothing;
  }
}

auto Program::get_uniform_names() const -> Vector<String>
{
  Vector<String> uniform_names;

  const auto uniform_count = get_uniform_count();
  uniform_names.reserve(static_cast<usize>(uniform_count));

  for (int uniform_index = 0; uniform_index < uniform_count; ++uniform_index) {
    char name_buffer[256] {};
    GLsizei name_length {};

    glGetActiveUniform(mID,
                       uniform_index,
                       sizeof name_buffer,
                       &name_length,
                       nullptr,
                       nullptr,
                       name_buffer);

    auto& name = uniform_names.emplace_back(name_buffer);
    name.reserve(static_cast<usize>(name_length));

    name = name_buffer;
  }

  return uniform_names;
}

auto Program::get_uniform_count() const -> int
{
  int uniform_count {};
  glGetProgramiv(mID, GL_ACTIVE_UNIFORMS, &uniform_count);
  return uniform_count;
}

}  // namespace glow::gl