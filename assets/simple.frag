#version 460 core

in vec3 vertex_color;
in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D tex;

void main()
{
    frag_color = texture(tex, tex_coord);
}
