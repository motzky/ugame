#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
in mat3 tbn;

out vec4 frag_color;

uniform sampler2D tex0;

void main()
{
    vec3 hdr_color = texture(tex0, tex_coord).rgb;

    vec3 mappeed = hdr_color / (hdr_color + vec3(1.0));

    mappeed = pow(mappeed, vec3(1.0/2.2));
    frag_color = vec4(mappeed, 1.0);
}
