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
    color.rgb = texture(texSampler, fragUV).rgb;
    float gray = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    color.rgb = mix(color.rgb, vec3(gray, gray, gray), amount);
    color.a = 1.0f;
}
)"
