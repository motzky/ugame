#pragma once

#include <ranges>
#include <span>
#include <vector>

#include "events/key_event.h"
#include "graphics/camera.h"
#include "graphics/cube_map.h"
#include "graphics/frame_buffer.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "loaders/mesh_loader.h"
#include "scene.h"
#include "tlv/tlv_reader.h"

namespace game
{
    class Renderer
    {
    public:
        Renderer(const TlvReader &reader, MeshLoader &mesh_loader, std::uint32_t width, std::uint32_t height, std::uint8_t samples = 1);
        auto render(const Camera &camera, const Scene &scene, float gamma) const -> void;

    private:
        struct FrameBufferData
        {
            FrameBufferData(std::vector<Texture> col_textures, Texture depth)
                : color_textures{std::move(col_textures)},
                  color_texture_pointers{color_textures |
                                         std::views::transform([](const auto &e) -> const Texture *
                                                               { return std::addressof(e); }) |
                                         std::ranges::to<std::vector>()},
                  depth_texture{std::move(depth)},
                  frame_buffer{color_texture_pointers, &depth_texture}
            {
            }
            std::vector<Texture> color_textures;
            std::vector<const Texture *> color_texture_pointers;
            Texture depth_texture;
            FrameBuffer frame_buffer;
        };

        Buffer _camera_buffer;
        Buffer _light_buffer;
        Mesh _skybox_cube;
        Material _skybox_material;
        Material _debug_line_material;
        FrameBufferData _main_framebuffer;
        FrameBufferData _ssao_framebuffer;
        FrameBufferData _post_processing_framebuffer_1;
        FrameBufferData _post_processing_framebuffer_2;
        FrameBufferData _depth_map_framebuffer;
        Mesh _sprite;
        Material _hdr_material;
        Material _grey_scale_material;
        Material _label_material;
        Material _blur_material;
        Material _ssao_material;
        Material _ssao_apply_material;
        Material _shadow_material;
        Camera _orth_camera;
    };
}