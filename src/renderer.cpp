#include "renderer.h"

#include "camera.h"
#include "entity.h"
#include "material.h"
#include "mesh.h"
#include "error.h"
#include "opengl.h"
#include "scene.h"

namespace game
{
    auto Renderer::render(const Camera &camera, const Scene &scene) const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto *entity : scene.entities)
        {
            const auto *mesh = entity->mesh();
            const auto *material = entity->material();

            ::glUseProgram(material->native_handle());

            const auto model_uniform = ::glGetUniformLocation(material->native_handle(), "model");
            ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

            ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, entity->model().data());

            const auto view_uniform = ::glGetUniformLocation(material->native_handle(), "view");
            ensure(view_uniform > -1, "failed to get location of uniform {}", "view");

            ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera.view().data());

            const auto proj_uniform = ::glGetUniformLocation(material->native_handle(), "projection");
            ensure(proj_uniform > -1, "failed to get location of uniform {}", "projection");

            ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera.projection().data());

            mesh->bind();
            ::glDrawArrays(GL_TRIANGLES, 0, 36);
            mesh->unbind();
        }
    }
}