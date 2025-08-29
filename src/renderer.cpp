#include "renderer.h"

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

    auto Renderer::render() const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT);

        ::glUseProgram(_material.native_handle());

        static constexpr auto model = Matrix4{Vector3{.x = .5f, .y = 0.f, .z = 0.f}};
        const auto model_uniform = ::glGetUniformLocation(_material.native_handle(), "model");
        ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model.data().data());

        static auto view = Matrix4::look_at({.x = 0.f, .y = 0.f, .z = 1.f}, {.x = 0.f, .y = 0.f, .z = 0.f}, {.x = 0.f, .y = 1.f, .z = 0.f});
        const auto view_uniform = ::glGetUniformLocation(_material.native_handle(), "view");
        ensure(view_uniform > -1, "failed to get location of uniform {}", "view");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, view.data().data());

        static constexpr auto proj = Matrix4{};
        const auto proj_uniform = ::glGetUniformLocation(_material.native_handle(), "projection");
        ensure(proj_uniform > -1, "failed to get location of uniform {}", "projection");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, proj.data().data());

        _mesh.bind();
        ::glDrawArrays(GL_TRIANGLES, 0, 3);
        _mesh.unbind();
    }
}