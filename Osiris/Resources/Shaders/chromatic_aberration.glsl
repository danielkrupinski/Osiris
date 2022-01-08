R"(
#version 130
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec2 fragUV;
out vec4 color;

layout(location = 0) uniform float amount;
layout(location = 1) uniform sampler2D texSampler;

void main()
{
    color.r = texture(texSampler, vec2(fragUV.x - amount, fragUV.y + amount)).r;
    color.g = texture(texSampler, fragUV).g;
    color.b = texture(texSampler, vec2(fragUV.x + amount, fragUV.y - amount)).b;
    color.a = 1.0f;
}
)"
