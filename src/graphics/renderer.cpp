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
        return game::Material{vert_name, vertex_shader, frag_name, fragment_shader};
    }

    auto generate_textures(std::size_t n, game::TextureUsage usage, std::uint32_t width, std::uint32_t height, std::uint8_t sample_count) -> std::vector<game::Texture>
    {
        auto textures = std::vector<game::Texture>{};

        std::ranges::generate_n(std::back_inserter(textures), n, [&]()
                                { return game::Texture{usage, width, height, sample_count}; });

        return textures;
    }

    auto model_matrix_for_label(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, std::uint32_t offset = 5)
    {
        return game::Matrix4{
            game::Vector3{static_cast<float>(x) + (width / 2.f) + offset,
                          -static_cast<float>(y) - (height / 2.f),
                          0.0f},
            game::Vector3{static_cast<float>(width) / 2.f,
                          static_cast<float>(height) / 2.f,
                          1.0f}};
    }

    auto blit_all(const game::FrameBuffer &src, ::GLenum src_attachment, const game::FrameBuffer &dst, ::GLenum dst_attachment, ::GLbitfield mask = GL_COLOR_BUFFER_BIT) -> void
    {
        ::glNamedFramebufferReadBuffer(src.native_handle(), src_attachment);
        ::glNamedFramebufferDrawBuffer(dst.native_handle(), dst_attachment);
        ::glBlitNamedFramebuffer(
            src.native_handle(),
            dst.native_handle(),
            0u,
            0u,
            src.width(),
            src.height(),
            0u,
            0u,
            dst.width(),
            dst.height(),
            mask,
            GL_NEAREST);
    }
    auto blit_all(const game::FrameBuffer &src, const game::FrameBuffer &dst, ::GLbitfield mask = GL_COLOR_BUFFER_BIT) -> void
    {
        ::glBlitNamedFramebuffer(
            src.native_handle(),
            dst.native_handle(),
            0u,
            0u,
            src.width(),
            src.height(),
            0u,
            0u,
            dst.width(),
            dst.height(),
            mask,
            GL_NEAREST);
    }

    auto apply_post_processing_effect(
        game::FrameBuffer const *&read_fb,
        game::FrameBuffer const *&write_fb,
        const game::Material &material,
        const game::TextureSampler *sampler,
        const game::Mesh &sprite,
        std::function<void(const game::Material &)> material_callback = nullptr) -> void
    {
        write_fb->bind();
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        material.use();
        material.bind_texture(0, read_fb->color_textures().front(), sampler);
        if (material_callback)
        {
            material_callback(material);
        }
        ::glDrawElements(GL_TRIANGLES, sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(sprite.index_offset()));

        std::ranges::swap(read_fb, write_fb);
    }

    auto write_camera_data_to_ubo(const game::Camera &camera, const game::Buffer &camera_buffer) -> void
    {
        auto writer = game::BufferWriter{camera_buffer};
        writer.write(camera.view());
        writer.write(camera.projection());
        writer.write(camera.position());
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
          _depth_map_framebuffer{{},
                                 {TextureUsage::DEPTH, 1024, 1024, 1}},
          _sprite{mesh_loader.sprite()},
          _hdr_material{create_material(reader, "hdr.vert", "hdr.frag")},
          _grey_scale_material{create_material(reader, "grey_scale.vert", "grey_scale.frag")},
          _label_material{create_material(reader, "label.vert", "label.frag")},
          _blur_material{create_material(reader, "blur.vert", "blur.frag")},
          _ssao_material{create_material(reader, "ssao.vert", "ssao.frag")},
          _ssao_apply_material{create_material(reader, "ssao.vert", "ssao_apply.frag")},
          _shadow_material{create_material(reader, "shadow.vert", "shadow.frag")},
          _orth_camera{static_cast<float>(width), static_cast<float>(height), 1000.f}
    {
        _orth_camera.set_position({width / 2.f, height / -2.f, 0.f});
        _orth_camera.update();
    }

    auto Renderer::render(const Camera &camera, const Scene &scene, float gamma) const -> void
    {
        _shadow_material.use();
        auto shadow_orth_camera = Camera{1024.f, 1024.f, 1000.f};
        shadow_orth_camera.set_position(scene.directional.direction);
        shadow_orth_camera.set_direction(-scene.directional.direction);
        write_camera_data_to_ubo(shadow_orth_camera, _camera_buffer);
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

        auto lightSpaceMatrix = Matrix4{shadow_orth_camera.projection()} * Matrix4{shadow_orth_camera.view()};

        ::glViewport(0, 0, 1024, 1024);
        _depth_map_framebuffer.frame_buffer.bind();
        ::glClear(GL_DEPTH_BUFFER_BIT);
        ::glCullFace(GL_FRONT);

        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            material->use();
            const auto model = Matrix4{entity->transform()};
            material->set_uniform("model", model);
            material->set_uniform("lightSpaceMatrix", lightSpaceMatrix);
            material->invoke_uniform_callback(entity);
            material->bind_textures(entity->textures());

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
        }

        _depth_map_framebuffer.frame_buffer.unbind();
        ::glViewport(0, 0, camera.width(), camera.height());
        ::glCullFace(GL_BACK);

        _main_framebuffer.frame_buffer.bind();
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        write_camera_data_to_ubo(camera, _camera_buffer);
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());
        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _light_buffer.native_handle());

        // for (const auto *entity : scene.entities | std::views::filter([](const auto *e)
        //                                                               { return e->is_visible(); }))
        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            material->use();
            const auto model = Matrix4{entity->transform()};
            material->set_uniform("model", model);
            material->set_uniform("lightSpaceMatrix", lightSpaceMatrix);
            material->invoke_uniform_callback(entity);
            material->bind_textures(entity->textures());
            material->bind_texture(99, &_depth_map_framebuffer.depth_texture, scene.skybox_sampler);

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
        }

        if (scene.skybox)
        {
            ::glDisable(GL_CULL_FACE);
            ::glDepthFunc(GL_LEQUAL);

            _skybox_cube.bind();

            _skybox_material.use();
            _skybox_material.bind_cube_map(scene.skybox, scene.skybox_sampler);

            ::glDrawElements(GL_TRIANGLES, _skybox_cube.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_skybox_cube.index_offset()));

            ::glDepthFunc(GL_LESS);
            ::glEnable(GL_CULL_FACE);
        }

        if (const auto &dbl = scene.debug_lines; dbl)
        {
            _debug_line_material.use();
            dbl->bind();
            ::glDrawArrays(GL_LINES, 0u, dbl->count());
        }

        _main_framebuffer.frame_buffer.unbind();

        auto *read_fb = &_post_processing_framebuffer_1.frame_buffer;
        auto *write_fb = &_post_processing_framebuffer_2.frame_buffer;

        _sprite.bind();
        if (scene.effects.ssao)
        {
            for (auto i = 0; i < 3; ++i)
            {
                blit_all(
                    _main_framebuffer.frame_buffer,
                    static_cast<::GLenum>(GL_COLOR_ATTACHMENT0 + i),
                    _ssao_framebuffer.frame_buffer,
                    static_cast<::GLenum>(GL_COLOR_ATTACHMENT0 + i));
            }
            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _ssao_material.use();
            _ssao_material.bind_texture(0, &_ssao_framebuffer.color_textures[1], scene.skybox_sampler);
            _ssao_material.bind_texture(1, &_ssao_framebuffer.color_textures[2], scene.skybox_sampler);
            _ssao_material.set_uniform("width", static_cast<float>(_ssao_framebuffer.frame_buffer.width()));
            _ssao_material.set_uniform("height", static_cast<float>(_ssao_framebuffer.frame_buffer.height()));

            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));

            std::ranges::swap(read_fb, write_fb);

            write_fb->bind();
            ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _ssao_apply_material.use();
            _ssao_apply_material.bind_texture(0, &_ssao_framebuffer.color_textures[0], scene.skybox_sampler);
            _ssao_apply_material.bind_texture(1, read_fb->color_textures().front(), scene.skybox_sampler);

            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));

            std::ranges::swap(read_fb, write_fb);
        }
        else
        {
            blit_all(_main_framebuffer.frame_buffer, _post_processing_framebuffer_1.frame_buffer);
            // blit_all(_main_framebuffer.frame_buffer, GL_COLOR_ATTACHMENT0, *write_fb, GL_COLOR_ATTACHMENT0, GL_DEPTH_BUFFER_BIT);
        }

        if (scene.effects.hdr)
        {
            apply_post_processing_effect(read_fb, write_fb, _hdr_material, scene.skybox_sampler, _sprite,
                                         [gamma](const auto &m)
                                         { m.set_uniform("gamma", gamma); });
        }

        if (scene.effects.grey_scale)
        {
            apply_post_processing_effect(read_fb, write_fb, _grey_scale_material, scene.skybox_sampler, _sprite);
        }

        if (scene.effects.blur)
        {
            apply_post_processing_effect(read_fb, write_fb, _blur_material, scene.skybox_sampler, _sprite,
                                         [write_fb](const auto &m)
                                         {
                                             m.set_uniform("screen_width", static_cast<float>(write_fb->width()));
                                             m.set_uniform("screen_height", static_cast<float>(write_fb->height()));
                                         });
        }

        if (!scene.labels.empty())
        {
            read_fb->bind();
            write_camera_data_to_ubo(_orth_camera, _camera_buffer);

            ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

            ::glEnable(GL_BLEND);
            ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            _label_material.use();
            for (const auto &[x, y, texture, color] : scene.labels)
            {
                const auto model = model_matrix_for_label(x, y, texture->width(), texture->height());
                _label_material.set_uniform("model", model);
                _label_material.set_uniform("textColor", color);

                _label_material.bind_texture(0, texture, scene.skybox_sampler);

                ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
            }
            ::glDisable(GL_BLEND);
        }

        _sprite.unbind();

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