#version 460 core

in vec3 normal;
in vec2 tex_coord;
in vec4 frag_position;
in mat3 tbn;

out vec4 frag_color;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform vec3 tint_color;
uniform float tint_amount;

layout(std140, binding = 0) uniform camera
{
    mat4 view;
    mat4 projection;
    vec3 eye;
};

struct PointLight
{
    vec3 point_light;
    vec3 point_color;
    vec3 attenuation;
};

layout(std430, binding = 1) readonly buffer lights
{
    vec3 ambient;
    vec3 direction;
    vec3 direction_color;
    int num_points;
    PointLight points[];
};


vec3 calc_ambient()
{
    return ambient;
}

vec3 calc_directional()
{
    vec3 light_dir = normalize(-direction);
    float diff = max(dot(normal, light_dir), 0.0);

    return diff * direction_color;
}

vec3 calc_point(int index)
{
    vec3 point_light = points[index].point_light;
    vec3 point_color = points[index].point_color;
    vec3 attenuation = points[index].attenuation;

    vec3 n = texture(tex2, tex_coord).rgb;
    n = (n * 2.0) - 1.0;
    n = normalize(tbn * n);

    float dist = length(point_light - frag_position.xyz);
    float att = 1.0 / (attenuation.x + (attenuation.y * dist) + (attenuation.z * (dist * dist)));

    vec3 light_dir = normalize(point_light - frag_position.xyz);
    float diff = max(dot(n, light_dir), 0.0);

    vec3 reflect_dir = reflect(-light_dir, n);
    float spec = pow(max(dot(normalize(eye - frag_position.xyz), reflect_dir), 0.0), 32) * texture(tex1, tex_coord).r;

    return ((diff + spec) * att) * point_color;
}

void main()
{
    vec3 norm = normalize(normal);

    vec4 albedo = texture(tex0, tex_coord);
    vec3 color = calc_ambient() * albedo.rgb;
    color += calc_directional();

    for(int i = 0; i < num_points; ++i)
    {
        color += calc_point(i);
    }

    frag_color = vec4(mix(color * albedo.rgb, tint_color, tint_amount), 1.0);
}
