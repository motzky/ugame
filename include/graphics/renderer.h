#pragma once

#include "camera.h"
#include "events/key_event.h"
#include "graphics/cube_map.h"
#include "graphics/frame_buffer.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture_sampler.h"
#include "loaders/mesh_loader.h"
#include "scene.h"
#include "tlv/tlv_reader.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer(const TlvReader &reader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height);
        auto render(const Camera &camera, const Scene &scene, float gamma) const -> void;

    private:
        Buffer _camera_buffer;
        Buffer _light_buffer;
        Mesh _skybox_cube;
        Material _skybox_material;
        Material _debug_line_material;
        FrameBuffer _fb;
        Mesh _post_process_sprite;
        Material _post_process_material;
    };
}