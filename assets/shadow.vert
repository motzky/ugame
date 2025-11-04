#version 460 core

layout (location = 0) in vec3 in_position;

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
    vec3 eye;
};

uniform mat4 model;


void main()
{
    mat4 lightSpaceMatrix = projection * view;
    gl_Position = lightSpaceMatrix * model * vec4(in_position, 1.0);
}