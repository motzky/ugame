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
#include "texture.h"
#include "texture_sampler.h"

namespace
{
    struct LightBuffer
    {
        alignas(16) game::Color ambient;
        alignas(16) game::Vector3 direction;
        alignas(16) game::Color direction_color;
        alignas(16) game::Vector3 point_postion;
        alignas(16) game::Color point_color;
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
                .point_color = scene.point.color};
            auto writer = BufferWriter{_light_buffer};
            writer.write(light_buffer);
        }

        ::glBindBufferBase(GL_UNIFORM_BUFFER, 1, _light_buffer.native_handle());

        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();
            const auto *sampler = entity->sampler();

            ::glUseProgram(material->native_handle());

            const auto model_uniform = ::glGetUniformLocation(material->native_handle(), "model");
            ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, entity->model().data());

            for (const auto &[index, tex] : entity->textures() | std::views::enumerate)
            {
                const auto gl_index = static_cast<::GLuint>(index);
                ::glBindTextureUnit(gl_index, tex->native_handle());
                ::glBindSampler(gl_index, sampler->native_handle());

                const auto uniform_name = std::format("tex{}", index);

                const auto tex_uniform = ::glGetUniformLocation(material->native_handle(), uniform_name.c_str());
                ensure(tex_uniform > -1, "failed to get location of uniform {}", uniform_name);
                ::glUniform1i(tex_uniform, 0);
            }

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
            mesh->unbind();
        }
    }
}