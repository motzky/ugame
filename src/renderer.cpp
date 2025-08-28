#include "renderer.h"

#include "material.h"

namespace game
{
    Renderer::Renderer(Material material)
        : _mesh{}, _material(std::move(material))
    {
        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    auto Renderer::render() const -> void
    {
        ::glClear(GL_COLOR_BUFFER_BIT);

        ::glUseProgram(_material.native_handle());
        _mesh.bind();
        ::glDrawArrays(GL_TRIANGLES, 0, 3);
        _mesh.unbind();
    }
}