#include "renderer.h"

#include <cstdint>
#include <ranges>

#include "buffer_writer.h"
#include "camera.h"
#include "color.h"
#include "cube_map.h"
#include "ensure.h"
#include "entity.h"
#include "frame_buffer.h"
#include "material.h"
#include "mesh.h"
#include "mesh_loader.h"
#include "opengl.h"
#include "resource_loader.h"
#include "scene.h"
#include "texture.h"
#include "texture_sampler.h"

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

    auto create_skybox_material(game::ResourceLoader &resource_loader) -> game::Material
    {
        const auto vertex_shader = game::Shader{resource_loader.load_string("cube.vert"), game::ShaderType::VERTEX};
        const auto fragment_shader = game::Shader{resource_loader.load_string("cube.frag"), game::ShaderType::FRAGMENT};
        return game::Material{vertex_shader, fragment_shader};
    }

}

namespace game
{
    Renderer::Renderer(ResourceLoader &resource_loader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height)
        : _camera_buffer(sizeof(Matrix4) * 2u + sizeof(Vector3)),
          _light_buffer(10240u),
          _skybox_cube(mesh_loader.cube()),
          _skybox_material(create_skybox_material(resource_loader)),
          _fb{width, height}
    {
    }

    auto Renderer::render(const Camera &camera, const Scene &scene, const CubeMap &skybox, const TextureSampler &skybox_sampler) -> void
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

        _skybox_material.bind_cube_map(&skybox, &skybox_sampler);

        _skybox_cube.unbind();
        ::glDepthMask(GL_TRUE);

        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            material->use();
            material->set_uniform("model", entity->model());
            material->bind_textures(entity->textures());

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
            mesh->unbind();
        }

        _fb.unbind();

        ::glBlitNamedFramebuffer(_fb.native_handle(), 0,
                                 0, 0, _fb.width(), _fb.height(),
                                 0, 0, _fb.width(), _fb.height(),
                                 GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
}