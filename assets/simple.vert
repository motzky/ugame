#version 460 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

out vec3 normal;
out vec2 tex_coord;
out vec4 frag_position;

uniform mat4 model;
layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
    vec3 eye;
};

void main()
{
    gl_Position = projection * view * model * vec4(in_position, 1.0);
    normal = (model * vec4(in_normal, 0)).xyz;
    tex_coord = in_uv;

    frag_position = model * vec4(in_position, 1.0);
}
