R"(
#version 130
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require

in vec2 fragUV;
out vec4 color;

layout(location = 0) uniform float texelWidth;
layout(location = 1) uniform sampler2D texSampler;

float offsets[3] = float[](0.0f, 1.3846153846f, 3.2307692308f);
float weights[3] = float[](0.2270270270f, 0.3162162162f, 0.0702702703f);

void main()
{
    color = texture(texSampler, fragUV);
    color.rgb *= weights[0];
    for (int i = 1; i < 3; ++i) {
        color.rgb += texture(texSampler, fragUV - vec2(texelWidth * offsets[i], 0.0f)).rgb * weights[i];
        color.rgb += texture(texSampler, fragUV + vec2(texelWidth * offsets[i], 0.0f)).rgb * weights[i];
    }
}
)"
