#include "graphics/renderer.h"

#include <cstdint>
#include <ranges>

#include "core/buffer_writer.h"
#include "graphics/camera.h"
#include "graphics/color.h"
#include "graphics/cube_map.h"
#include "graphics/frame_buffer.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/opengl.h"
#include "graphics/scene.h"
#include "graphics/texture.h"
#include "graphics/texture_sampler.h"
#include "loaders/mesh_loader.h"
#include "primitives/entity.h"
#include "tlv/tlv_reader.h"
#include "utils/ensure.h"

namespace
{

#if defined(_MSC_VER)
// disable weird warning about alingas in MSVC
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

    struct PointLightBuffer
    {
        alignas(16) game::Vector3 point_postion;
        alignas(16) game::Color point_color;
        alignas(16) game::Vector3 attenuation;
    };

    struct LightBuffer
    {
        alignas(16) game::Color ambient;
        alignas(16) game::Vector3 direction;
        alignas(16) game::Color direction_color;
        int num_points;
    };
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    auto create_material(const game::TlvReader &reader, std::string_view vert_name, std::string_view frag_name) -> game::Material
    {
        const auto vert_file = game::TlvReader::get_text_file(reader, vert_name);
        const auto vert_data = vert_file.data;
        const auto vertex_shader = game::Shader{vert_data, game::ShaderType::VERTEX};

        const auto frag_file = game::TlvReader::get_text_file(reader, frag_name);
        const auto frag_data = frag_file.data;
        const auto fragment_shader = game::Shader{frag_data, game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }

    auto generate_textures(std::size_t n, game::TextureUsage usage, std::uint32_t width, std::uint32_t height, std::uint8_t sample_count) -> std::vector<game::Texture>
    {
        auto textures = std::vector<game::Texture>{};

        std::ranges::generate_n(std::back_inserter(textures), n, [&]()
                                { return game::Texture{usage, width, height, sample_count}; });

        return textures;
    }
}

namespace game
{
    Renderer::Renderer(const TlvReader &reader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height, std::uint8_t samples)
        : _camera_buffer(sizeof(Matrix4) * 2u + sizeof(Vector3)),
          _light_buffer(10240u),
          _skybox_cube(mesh_loader.cube()),
          _skybox_material(create_material(reader, "cube.vert", "cube.frag")),
          _debug_line_material(create_material(reader, "line.vert", "line.frag")),
          _main_framebuffer{generate_textures(3uz, TextureUsage::FRAMEBUFFER, width, height, samples),
                            {TextureUsage::DEPTH, width, height, samples}},
          _ssao_framebuffer{generate_textures(3uz, TextureUsage::FRAMEBUFFER, width, height, 1),
                            {TextureUsage::DEPTH, width, height, 1}},
          _post_processing_framebuffer_1{generate_textures(1zu, TextureUsage::FRAMEBUFFER, width, height, 1),
                                         {TextureUsage::DEPTH, width, height, 1}},
          _post_processing_framebuffer_2{generate_textures(1zu, TextureUsage::FRAMEBUFFER, width, height, 1),
                                         {TextureUsage::DEPTH, width, height, 1}},
          _sprite{mesh_loader.sprite()},
          _hdr_material{create_material(reader, "hdr.vert", "hdr.frag")},
          _grey_scale_material{create_material(reader, "grey_scale.vert", "grey_scale.frag")},
          _label_material{create_material(reader, "label.vert", "label.frag")},
          _blur_material{create_material(reader, "blur.vert", "blur.frag")},
          _ssao_material{create_material(reader, "ssao.vert", "ssao.frag")},
          _ssao_apply_material{create_material(reader, "ssao.vert", "ssao_apply.frag")},
          _orth_camera{static_cast<float>(width), static_cast<float>(height), 1000.f}
    {
        _orth_camera.set_position({width / 2.f, height / -2.f, 0.f});
        _orth_camera.update();
    }

    auto Renderer::render(const Camera &camera, const Scene &scene, float gamma) const -> void
    {
        _main_framebuffer.frame_buffer.bind();

        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            auto writer = BufferWriter{_camera_buffer};
            writer.write(camera.view());
            writer.write(camera.projection());
            writer.write(camera.position());
        }
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

        {
            auto light_buffer = LightBuffer{
                .ambient = scene.ambient,
                .direction = scene.directional.direction,
                .direction_color = scene.directional.color,
                .num_points = static_cast<int>(scene.points.size())};
            auto writer = BufferWriter{_light_buffer};
            writer.write(light_buffer);
            for (const auto &point : scene.points)
            {
                auto point_light_bufer = PointLightBuffer{
                    .point_postion = point.position,
                    .point_color = point.color,
                    .attenuation = {point.const_attenuation, point.linear_attenuation, point.quad_attenuation}};
                writer.write(point_light_bufer);
            }
        }

        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _light_buffer.native_handle());

        if (scene.skybox)
        {
            ::glDisable(GL_CULL_FACE);
            ::glDepthMask(GL_FALSE);

            _skybox_material.use();
            _skybox_cube.bind();

            ::glDrawElements(GL_TRIANGLES, _skybox_cube.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_skybox_cube.index_offset()));

            _skybox_material.bind_cube_map(scene.skybox, scene.skybox_sampler);

            _skybox_cube.unbind();

            ::glDepthMask(GL_TRUE);
            ::glEnable(GL_CULL_FACE);
        }

        // for (const auto *entity : scene.entities | std::views::filter([](const auto *e)
        //                                                               { return e->is_visible(); }))
        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            material->use();
            const auto model = Matrix4{entity->transform()};
            material->set_uniform("model", model);
            material->invoke_uniform_callback(entity);
            material->bind_textures(entity->textures());

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
            mesh->unbind();
        }

        if (const auto &dbl = scene.debug_lines; dbl)
        {
            _debug_line_material.use();
            dbl->bind();
            ::glDrawArrays(GL_LINES, 0u, dbl->count());
            dbl->unbind();
        }

        _main_framebuffer.frame_buffer.unbind();

        auto *read_fb = &_post_processing_framebuffer_1.frame_buffer;
        auto *write_fb = &_post_processing_framebuffer_2.frame_buffer;

        if (scene.effects.ssao)
        {
            for (auto i = 0; i < 3; ++i)
            {
                ::glNamedFramebufferReadBuffer(_main_framebuffer.frame_buffer.native_handle(), static_cast<::GLenum>(GL_COLOR_ATTACHMENT0 + i));
                ::glNamedFramebufferDrawBuffer(_ssao_framebuffer.frame_buffer.native_handle(), static_cast<::GLenum>(GL_COLOR_ATTACHMENT0 + i));
                ::glBlitNamedFramebuffer(
                    _main_framebuffer.frame_buffer.native_handle(),
                    _ssao_framebuffer.frame_buffer.native_handle(),
                    0u,
                    0u,
                    _main_framebuffer.frame_buffer.width(),
                    _main_framebuffer.frame_buffer.height(),
                    0u,
                    0u,
                    _ssao_framebuffer.frame_buffer.width(),
                    _ssao_framebuffer.frame_buffer.height(),
                    GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);
            }
            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _ssao_material.use();
            _ssao_material.bind_texture(0, &_ssao_framebuffer.color_textures[1], scene.skybox_sampler);
            _ssao_material.bind_texture(1, &_ssao_framebuffer.color_textures[2], scene.skybox_sampler);
            _ssao_material.set_uniform("width", static_cast<float>(_ssao_framebuffer.frame_buffer.width()));
            _ssao_material.set_uniform("height", static_cast<float>(_ssao_framebuffer.frame_buffer.height()));

            _sprite.bind();
            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            _sprite.unbind();

            write_fb->unbind();
            std::ranges::swap(read_fb, write_fb);

            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _ssao_apply_material.use();
            _ssao_apply_material.bind_texture(0, &_ssao_framebuffer.color_textures[0], scene.skybox_sampler);
            _ssao_apply_material.bind_texture(1, read_fb->color_textures().front(), scene.skybox_sampler);

            _sprite.bind();
            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            _sprite.unbind();

            write_fb->unbind();
            std::ranges::swap(read_fb, write_fb);
        }
        else
        {
            ::glBlitNamedFramebuffer(
                _main_framebuffer.frame_buffer.native_handle(),
                _post_processing_framebuffer_1.frame_buffer.native_handle(),
                0u,
                0u,
                _main_framebuffer.frame_buffer.width(),
                _main_framebuffer.frame_buffer.height(),
                0u,
                0u,
                _post_processing_framebuffer_1.frame_buffer.width(),
                _post_processing_framebuffer_1.frame_buffer.height(),
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
        }

        if (scene.effects.hdr)
        {
            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _hdr_material.use();
            _hdr_material.bind_texture(0, read_fb->color_textures().front(), scene.skybox_sampler);
            _hdr_material.set_uniform("gamma", gamma);

            _sprite.bind();
            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            _sprite.unbind();

            write_fb->unbind();
            std::ranges::swap(read_fb, write_fb);
        }

        if (scene.effects.grey_scale)
        {
            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _grey_scale_material.use();
            _grey_scale_material.bind_texture(0, read_fb->color_textures().front(), scene.skybox_sampler);

            _sprite.bind();
            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            _sprite.unbind();

            write_fb->unbind();
            std::ranges::swap(read_fb, write_fb);
        }

        if (scene.effects.blur)
        {
            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _blur_material.use();
            _blur_material.bind_texture(0, read_fb->color_textures().front(), scene.skybox_sampler);
            _blur_material.set_uniform("screen_width", static_cast<float>(write_fb->width()));
            _blur_material.set_uniform("screen_height", static_cast<float>(write_fb->height()));

            _sprite.bind();
            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            _sprite.unbind();

            write_fb->unbind();
            std::ranges::swap(read_fb, write_fb);
        }

        read_fb->bind();

        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _label_material.use();
        _sprite.bind();

        if (!scene.labels.empty())
        {
            auto writer = BufferWriter{_camera_buffer};
            writer.write(_orth_camera.view());
            writer.write(_orth_camera.projection());
            writer.write(_orth_camera.position());
        }

        for (const auto &[x, y, texture, color] : scene.labels)
        {
            ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

            const auto model = Matrix4{
                Vector3{
                    static_cast<float>(x) + (texture->width() / 2.f) + 5,
                    -static_cast<float>(y) - (texture->height() / 2.f),
                    0.0f},
                Vector3{static_cast<float>(texture->width()) / 2.f, static_cast<float>(texture->height()) / 2.f, 1.0f}};
            _label_material.set_uniform("model", model);
            _label_material.set_uniform("textColor", color);

            _label_material.bind_texture(0, texture, scene.skybox_sampler);

            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
        }
        _sprite.unbind();

        ::glDisable(GL_BLEND);

        read_fb->unbind();

        ::glBlitNamedFramebuffer(
            read_fb->native_handle(),
            0u,
            0u,
            0u,
            read_fb->width(),
            read_fb->height(),
            0u,
            0u,
            read_fb->width(),
            read_fb->height(),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);
    }
}