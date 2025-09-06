#pragma once

#include "buffer.h"
#include "camera.h"
#include "cube_map.h"
#include "frame_buffer.h"
#include "material.h"
#include "mesh.h"
#include "mesh_loader.h"
#include "resource_loader.h"
#include "scene.h"
#include "texture_sampler.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer(ResourceLoader &resource_loader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height);
        auto render(const Camera &camera, const Scene &scene, const CubeMap &skybox, const TextureSampler &skybox_sampler, float gamma) const -> void;

    private:
        Buffer _camera_buffer;
        Buffer _light_buffer;
        Mesh _skybox_cube;
        Material _skybox_material;
        FrameBuffer _fb;
        Mesh _post_process_sprite;
        Material _post_process_material;
    };
}