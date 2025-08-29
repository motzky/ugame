#include "renderer.h"

#include <cmath>
#include <numbers>

#include "error.h"
#include "material.h"
#include "matrix4.h"
#include "vector3.h"

namespace game
{
    Renderer::Renderer(Material material)
        : _material(std::move(material)), _mesh{}
    {
        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    auto Renderer::render(const Camera &camera) const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ::glUseProgram(_material.native_handle());

        static constexpr auto model = Matrix4{Vector3{.x = 0.f, .y = 0.f, .z = 0.f}};
        const auto model_uniform = ::glGetUniformLocation(_material.native_handle(), "model");
        ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model.data().data());

        const auto view_uniform = ::glGetUniformLocation(_material.native_handle(), "view");
        ensure(view_uniform > -1, "failed to get location of uniform {}", "view");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, camera.view().data());

        const auto proj_uniform = ::glGetUniformLocation(_material.native_handle(), "projection");
        ensure(proj_uniform > -1, "failed to get location of uniform {}", "projection");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, camera.projection().data());

        _mesh.bind();
        ::glDrawArrays(GL_TRIANGLES, 0, 36);
        _mesh.unbind();
    }
}