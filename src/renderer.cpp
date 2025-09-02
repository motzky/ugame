#include "renderer.h"

#include "buffer_writer.h"
#include "camera.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "ensure.h"
#include "opengl.h"
#include "scene.h"

namespace game
{
    Renderer::Renderer()
        : _camera_buffer(sizeof(Matrix4) * 2u)
    {
    }

    auto Renderer::render(const Camera &camera, const Scene &scene) const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            auto writer = BufferWriter{_camera_buffer};
            writer.write(camera.view());
            writer.write(camera.projection());
        }

        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, _camera_buffer.native_handle());

        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            ::glUseProgram(material->native_handle());

            const auto model_uniform = ::glGetUniformLocation(material->native_handle(), "model");
            ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, entity->model().data());

            mesh->bind();
            ::glDrawElements(GL_TRIANGLES, mesh->index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh->index_offset()));
            mesh->unbind();
        }
    }
}