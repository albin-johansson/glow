#include "texture_2d.hpp"

#include <cassert>  // assert

#include <glad/glad.h>

#include "graphics/opengl/util.hpp"

namespace gravel::gl {

Texture2D::Texture2D()
{
  glGenTextures(1, &mID);
  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  unbind();
  GRAVEL_GL_CHECK_ERRORS();
}

Texture2D::~Texture2D() noexcept
{
  dispose();
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : mID {other.mID}
{
  other.mID = 0;
}

auto Texture2D::operator=(Texture2D&& other) noexcept -> Texture2D&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    other.mID = 0;
  }

  return *this;
}

void Texture2D::dispose() noexcept
{
  if (mID != 0) {
    glDeleteTextures(1, &mID);
  }
}

void Texture2D::bind()
{
  glBindTexture(GL_TEXTURE_2D, mID);
  GRAVEL_GL_CHECK_ERRORS();
}

void Texture2D::bind(const uint id)
{
  glBindTexture(GL_TEXTURE_2D, id);
  GRAVEL_GL_CHECK_ERRORS();
}

void Texture2D::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
  GRAVEL_GL_CHECK_ERRORS();
}

void Texture2D::set_data(const int detail_level,
                         const int texture_format,
                         const uint pixel_format,
                         const uint type,
                         const Vec2i& size,
                         const void* pixel_data)
{
  assert(get_bound_texture() == mID);
  glTexImage2D(GL_TEXTURE_2D,
               detail_level,
               texture_format,
               size.x,
               size.y,
               0,
               pixel_format,
               type,
               pixel_data);
  GRAVEL_GL_CHECK_ERRORS();
}

}  // namespace gravel::gl