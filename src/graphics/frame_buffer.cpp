#include "graphics/frame_buffer.h"

#include <cstdint>

#include "auto_release.h"
#include "ensure.h"
#include "graphics/opengl.h"
#include "graphics/texture.h"

namespace game
{
    FrameBuffer::FrameBuffer(std::uint32_t width, std::uint32_t height)
        : _handle{0u, [](const auto buffer)
                  { ::glDeleteFramebuffers(1, &buffer); }},
          _width(width),
          _height(height),
          _color_texture{TextureUsage::FRAMEBUFFER, width, height},
          _depth_texture{TextureUsage::DEPTH, width, height}
    {
        ::glCreateFramebuffers(1, &_handle);
        ::glNamedFramebufferTexture(_handle, GL_COLOR_ATTACHMENT0, _color_texture.native_handle(), 0);
        ::glNamedFramebufferTexture(_handle, GL_DEPTH_ATTACHMENT, _depth_texture.native_handle(), 0);
    }

    auto FrameBuffer::native_handle() const -> ::GLuint
    {
        return _handle;
    }

    auto FrameBuffer::bind() const -> void
    {
        ::glBindFramebuffer(GL_FRAMEBUFFER, _handle);
    }

    auto FrameBuffer::unbind() const -> void
    {
        ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    }

    auto FrameBuffer::width() const -> std::uint32_t
    {
        return _width;
    }

    auto FrameBuffer::height() const -> std::uint32_t
    {
        return _height;
    }

    auto FrameBuffer::color_texture() const -> const Texture &
    {
        return _color_texture;
    }

}