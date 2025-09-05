#pragma once

#include "buffer.h"
#include "camera.h"
#include "cube_map.h"
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
        Renderer(ResourceLoader &resource_loader, MeshLoader &mesh_loader);
        auto render(const Camera &camera, const Scene &scene, const CubeMap &skybox, const TextureSampler &skybox_sampler) const -> void;

    private:
        Buffer _camera_buffer;
        Buffer _light_buffer;
        Mesh _skybox_cube;
        Material _skybox_material;
    };
}