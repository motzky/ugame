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

    auto Renderer::render() const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT);

        ::glUseProgram(_material.native_handle());

        static auto x = 2.f;
        static auto z = 0.f;
        static auto t = 0.f;
        static auto delta = .1f;

        x = std::sin(t) * 2.f;
        z = std::cos(t) * 2.f;

        t += delta;

        static constexpr auto model = Matrix4{Vector3{.x = 0.f, .y = 0.f, .z = 0.f}};
        const auto model_uniform = ::glGetUniformLocation(_material.native_handle(), "model");
        ensure(model_uniform > -1, "failed to get location of uniform {}", "model");

        ::glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model.data().data());

        const auto view = Matrix4::look_at({.x = x, .y = 0.f, .z = z}, {.x = 0.f, .y = 0.f, .z = 0.f}, {.x = 0.f, .y = 1.f, .z = 0.f});
        const auto view_uniform = ::glGetUniformLocation(_material.native_handle(), "view");
        ensure(view_uniform > -1, "failed to get location of uniform {}", "view");

        ::glUniformMatrix4fv(view_uniform, 1, GL_FALSE, view.data().data());

        static const auto proj = Matrix4::perspective(std::numbers::pi_v<float> / 4.f, 800.f, 600.f, 0.001f, 100.f);
        const auto proj_uniform = ::glGetUniformLocation(_material.native_handle(), "projection");
        ensure(proj_uniform > -1, "failed to get location of uniform {}", "projection");

        ::glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, proj.data().data());

        _mesh.bind();
        ::glDrawArrays(GL_TRIANGLES, 0, 3);
        _mesh.unbind();
    }
}