#version 460 core

in vec2 tex_coord;

layout (location = 0) out vec4 frag_color;

uniform sampler2D tex0; // color
uniform sampler2D tex1; // ssao

void main()
{
    // first blur the ssao texture
    vec2 texelSize = 1.0 / vec2(textureSize(tex1, 0));
    float result = 0.0;
    for (int x = -2; x<2; ++x)
    {
        for(int y=-2; y<2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(tex1, tex_coord + offset).r;
        }
    }

    // and then combine the blur with the color fragments of the rendered scene
    vec4 color = texture(tex0, tex_coord);

     frag_color = color * result / 16.0;
}
