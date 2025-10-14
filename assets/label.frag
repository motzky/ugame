#version 460 core

in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D tex0;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex0, tex_coord).r);
    frag_color = vec4(textColor, 1.0) * sampled;
}
