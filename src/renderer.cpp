#include "renderer.h"

#include <ranges>

#include "buffer_writer.h"
#include "camera.h"
#include "color.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "ensure.h"
#include "opengl.h"
#include "scene.h"

namespace
{
    struct LightBuffer
    {
        alignas(16) game::Color ambient;
        alignas(16) game::Vector3 direction;
        alignas(16) game::Color direction_color;
        alignas(16) game::Vector3 point_postion;
        alignas(16) game::Color point_color;
        alignas(16) game::Vector3 attenuation;
    };
}

namespace game
{
    Renderer::Renderer()
        : _camera_buffer(sizeof(Matrix4) * 2u + sizeof(Vector3)),
          _light_buffer(sizeof(LightBuffer))
    {
    }

    auto Renderer::render(const Camera &camera, const Scene &scene) const -> void
    {
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
                .point_postion = scene.point.position,
                .point_color = scene.point.color,
                .attenuation = {scene.point.const_attenuation, scene.point.linear_attenuation, scene.point.quad_attenuation}};
            auto writer = BufferWriter{_light_buffer};
            writer.write(light_buffer);
        }

        ::glBindBufferBase(GL_UNIFORM_BUFFER, 1, _light_buffer.native_handle());

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
    }
}