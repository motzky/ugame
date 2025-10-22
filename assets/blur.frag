#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
in mat3 tbn;

out vec4 frag_color;

uniform sampler2D tex0;
uniform float screen_width;
uniform float screen_height;

void main()
{
    const int KERNEL_SIZE = 13;
    const float weights[KERNEL_SIZE] = float[] (
        0.070159, 0.080661, 0.085998,
        0.087449, 0.086998, 0.080661,
        0.070159, 0.080661, 0.085998,
        0.087449, 0.086998, 0.080661,
        0.070159
    );

    const vec2 offsets[KERNEL_SIZE] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.5), vec2( 1.0, -1.0),
        vec2(-1.5,  0.0), vec2(0.0,  0.0), vec2( 1.5,  0.0),
        vec2(-1.0,  1.0), vec2(0.0,  1.5), vec2( 1.0,  1.0),
        vec2(-2.0,  0.5), vec2(2.0, -0.5), vec2(-0.5,  2.0),
        vec2( 0.5, -2.0)
    );

    vec3 color = vec3(0.0);
    vec2 scale = vec2(4.0) * vec2(1.0/screen_width, 1.0/screen_height);

    for(int i = 0; i < KERNEL_SIZE; ++i)
    {
        color += texture(tex0, tex_coord + offsets[i] * scale).rgb * weights[i];
    }

    frag_color = vec4(color, 1.0);
}
