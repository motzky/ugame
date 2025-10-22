#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
in mat3 tbn;

out vec4 frag_color;

uniform sampler2D tex0;

void main()
{
    vec3 input_color = texture(tex0, tex_coord).rgb;

    float grey = (0.299 * input_color.r) + (0.587 * input_color.g) + (0.114 * input_color.b);

    frag_color = vec4(grey, grey, grey, 1.0);
}
