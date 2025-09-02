#pragma once

#include "buffer.h"
#include "camera.h"
#include "scene.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer();
        auto render(const Camera &camera, const Scene &scene) const -> void;

    private:
        Buffer _camera_buffer;
        Buffer _light_buffer;
    };
}