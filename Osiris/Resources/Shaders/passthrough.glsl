R"(
#version 130
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 fragUV;

void main()
{
    fragUV = uv;
    gl_Position = vec4(pos.xy, 0, 1);
}
)"
