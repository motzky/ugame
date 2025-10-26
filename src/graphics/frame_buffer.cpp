#include "graphics/frame_buffer.h"

#include <cstdint>

#include "graphics/opengl.h"
#include "graphics/texture.h"
#include "utils/auto_release.h"
#include "utils/ensure.h"

namespace game
{
    FrameBuffer::FrameBuffer(std::uint32_t width, std::uint32_t height, std::uint8_t samples)
        : _handle{0u, [](const auto buffer)
                  { ::glDeleteFramebuffers(1, &buffer); }},
          _width(width),
          _height(height),
          _color_texture{TextureUsage::FRAMEBUFFER, width, height, samples},
          _depth_texture{TextureUsage::DEPTH, width, height, samples}
    {
        ::glCreateFramebuffers(1, &_handle);

        ::glNamedFramebufferTexture(_handle, GL_COLOR_ATTACHMENT0, _color_texture.native_handle(), 0);
        ::glNamedFramebufferTexture(_handle, GL_DEPTH_ATTACHMENT, _depth_texture.native_handle(), 0);

        // ::GLuint rbs[] = {GLuint{}, GLuint{}};
        // ::glCreateRenderbuffers(2u, rbs);
        // ::glNamedRenderbufferStorageMultisample(rbs[0], 4, GL_RGB16F, width, height);
        // ::glNamedRenderbufferStorageMultisample(rbs[1], 4, GL_DEPTH_COMPONENT24, width, height);

        // ::glNamedFramebufferRenderbuffer(_handle, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbs[0]);
        // ::glNamedFramebufferRenderbuffer(_handle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbs[1]);

        auto status = ::glCheckNamedFramebufferStatus(_handle, GL_FRAMEBUFFER);
        expect(status == GL_FRAMEBUFFER_COMPLETE, "framebuffer is not complete");
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