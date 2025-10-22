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

    auto create_skybox_material(const game::TlvReader &reader) -> game::Material
    {
        const auto vert_file = game::TlvReader::get_text_file(reader, "cube.vert");
        const auto vert_data = vert_file.data;
        const auto vertex_shader = game::Shader{vert_data, game::ShaderType::VERTEX};

        const auto frag_file = game::TlvReader::get_text_file(reader, "cube.frag");
        const auto frag_data = frag_file.data;
        const auto fragment_shader = game::Shader{frag_data, game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }

    auto create_post_process_material(const game::TlvReader &reader) -> game::Material
    {
        const auto vert_file = game::TlvReader::get_text_file(reader, "post_process.vert");
        const auto vert_data = vert_file.data;

        const auto vertex_shader = game::Shader{vert_data, game::ShaderType::VERTEX};

        const auto frag_file = game::TlvReader::get_text_file(reader, "post_process.frag");
        const auto frag_data = frag_file.data;

        const auto fragment_shader = game::Shader{frag_data, game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }

    auto create_line_material(const game::TlvReader &reader) -> game::Material
    {
        const auto vert_file = game::TlvReader::get_text_file(reader, "line.vert");
        const auto vert_data = vert_file.data;
        const auto vertex_shader = game::Shader{vert_data, game::ShaderType::VERTEX};

        const auto frag_file = game::TlvReader::get_text_file(reader, "line.frag");
        const auto frag_data = frag_file.data;
        const auto fragment_shader = game::Shader{frag_data, game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }

    auto create_label_material(const game::TlvReader &reader) -> game::Material
    {
        const auto vert_file = game::TlvReader::get_text_file(reader, "label.vert");
        const auto vert_data = vert_file.data;
        const auto vertex_shader = game::Shader{vert_data, game::ShaderType::VERTEX};

        const auto frag_file = game::TlvReader::get_text_file(reader, "label.frag");
        const auto frag_data = frag_file.data;
        const auto fragment_shader = game::Shader{frag_data, game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }
}

namespace game
{
    Renderer::Renderer(const TlvReader &reader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height)
        : _camera_buffer(sizeof(Matrix4) * 2u + sizeof(Vector3)),
          _light_buffer(10240u),
          _skybox_cube(mesh_loader.cube()),
          _skybox_material(create_skybox_material(reader)),
          _debug_line_material(create_line_material(reader)),
          _fb{width, height},
          _sprite{mesh_loader.sprite()},
          _post_process_material{create_post_process_material(reader)},
          _label_material{create_label_material(reader)},
          _orth_camera{static_cast<float>(width), static_cast<float>(height), 1000.f}
    {
        _orth_camera.set_position({width / 2.f, height / -2.f, 0.f});
        _orth_camera.update();
    }

    auto Renderer::render(const Camera &camera, const Scene &scene, float gamma) const -> void
    {
        _fb.bind();

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

        ::glDepthMask(GL_FALSE);

        _skybox_material.use();
        _skybox_cube.bind();

        ::glDrawElements(GL_TRIANGLES, _skybox_cube.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_skybox_cube.index_offset()));

        _skybox_material.bind_cube_map(scene.skybox, scene.skybox_sampler);

        _skybox_cube.unbind();

        ::glDepthMask(GL_TRUE);

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

        _fb.unbind();

        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _post_process_material.use();
        _post_process_material.bind_texture(0, &_fb.color_texture(), scene.skybox_sampler);
        _post_process_material.set_uniform("gamma", gamma);

        _sprite.bind();
        ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
        _sprite.unbind();

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

        for (const auto &[x, y, texture] : scene.labels)
        {
            ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

            const auto model = Matrix4{
                Vector3{
                    static_cast<float>(x) + (texture->width() / 2.f) + 5,
                    static_cast<float>(y) - (texture->height() / 2.f),
                    0.0f},
                Vector3{static_cast<float>(texture->width()) / 2.f, static_cast<float>(texture->height()) / 2.f, 1.0f}};
            _label_material.set_uniform("model", model);
            _label_material.set_uniform("textColor", Color::white());

            _label_material.bind_texture(0, texture, scene.skybox_sampler);

            ::glDrawElements(GL_TRIANGLES, _sprite.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(_sprite.index_offset()));
        }
        _sprite.unbind();

        ::glDisable(GL_BLEND);
    }
}