#include "framebuffer.hpp"

#include <utility>  // move

#include <glad/glad.h>

#include "common/debug/assert.hpp"
#include "graphics/opengl/util.hpp"

namespace gravel::gl {

Framebuffer::Framebuffer()
{
  glGenFramebuffers(1, &mID);
  bind();

  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         mColorTexture.get_id(),
                         0);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D,
                         mDepthTexture.get_id(),
                         0);

  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  unbind();

  GRAVEL_GL_CHECK_ERRORS();
}

Framebuffer::~Framebuffer() noexcept
{
  dispose();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : mID {other.mID},
      mColorTexture {std::move(other.mColorTexture)},
      mDepthTexture {std::move(other.mDepthTexture)},
      mSize {other.mSize}
{
  other.mID = 0;
}

auto Framebuffer::operator=(Framebuffer&& other) noexcept -> Framebuffer&
{
  if (this != &other) {
    dispose();

    mID = other.mID;
    mColorTexture = std::move(other.mColorTexture);
    mDepthTexture = std::move(other.mDepthTexture);
    mSize = other.mSize;

    other.mID = 0;
  }

  return *this;
}

void Framebuffer::dispose() noexcept
{
  if (mID != 0) {
    glDeleteFramebuffers(1, &mID);
  }
}

void Framebuffer::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, mID);
}

void Framebuffer::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(const Vec2i size)
{
  GRAVEL_ASSERT(get_bound_framebuffer() == mID);

  if (mSize == size) {
    return;
  }

  mSize = size;

  mColorTexture.bind();
  mColorTexture.set_data(0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, mSize, nullptr);

  mDepthTexture.bind();
  mDepthTexture.set_data(0,  //
                         GL_DEPTH_COMPONENT24,
                         GL_DEPTH_COMPONENT,
                         GL_FLOAT,
                         mSize,
                         nullptr);

  Texture2D::unbind();
  GRAVEL_GL_CHECK_ERRORS();
}

void Framebuffer::clear()
{
  GRAVEL_ASSERT(get_bound_framebuffer() == mID);
  GRAVEL_ASSERT_MSG(is_complete(), "Incomplete framebuffer used, remember to set size");

  glViewport(0, 0, mSize.x, mSize.y);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GRAVEL_GL_CHECK_ERRORS();
}

auto Framebuffer::is_complete() const -> bool
{
  GRAVEL_ASSERT(get_bound_framebuffer() == mID);
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

auto Framebuffer::get_id() const -> uint
{
  return mID;
}

auto Framebuffer::get_color_texture_id() const -> uint
{
  return mColorTexture.get_id();
}

auto Framebuffer::get_depth_texture_id() const -> uint
{
  return mDepthTexture.get_id();
}

auto Framebuffer::get_size() const -> const Vec2i&
{
  return mSize;
}

}  // namespace gravel::gl