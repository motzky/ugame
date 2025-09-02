#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;

out vec4 frag_color;

uniform sampler2D tex0;
uniform sampler2D tex1;

layout(std140, binding = 1) uniform lights
{
    vec3 ambient;
    vec3 direction;
    vec3 direction_color;
    vec3 point_light;
    vec3 point_color;
};

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
    vec3 eye;
};


vec3 calc_ambient()
{
    return ambient;
}

vec3 calc_directional(vec3 norm)
{
    vec3 light_dir = normalize(-direction);
    float diff = max(dot(norm, light_dir), 0.0);

    return diff * direction_color;
}

vec3 calc_point_diffuse(vec3 light_dir, vec3 norm)
{
    float diff = max(dot(norm, light_dir), 0.0);
    return diff * point_color;
}

vec3 calc_point_specular(vec3 light_dir, vec3 norm)
{
    vec3 view_dir = normalize(eye - frag_position.xyz);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    // return spec * point_color;
    return spec * vec3(1,1,1);
}

void main()
{
    vec3 norm = normalize(normal);

    vec4 albedo = texture(tex0, tex_coord);
    vec3 amb_color = calc_ambient() * albedo.rgb;
    vec3 dir_color = calc_directional(norm);

    vec3 point_light_dir = normalize(point_light - frag_position.xyz);

    vec3 point_diffuse = calc_point_diffuse(point_light_dir, norm);
    vec3 point_specular = calc_point_specular(point_light_dir, norm);

    vec3 diffuse = (dir_color + point_diffuse) * albedo.rgb;
    vec3 specular = point_specular * texture(tex1, tex_coord).rgb;

    frag_color = vec4(amb_color + diffuse + specular, 1.0);
}
