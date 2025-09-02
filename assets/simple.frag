#version 460 core

in vec3 vertex_color;
in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D tex;
layout(std140, binding = 1) uniform lights
{
    vec3 ambient;
};


void main()
{
    frag_color = vec4(ambient, 1.0) * texture(tex, tex_coord);
}
