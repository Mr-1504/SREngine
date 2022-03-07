#version 450

#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec2 TexCoord;
layout (location = 1) in vec4 Color;

layout (binding = 2) uniform sampler2D DiffuseMap;

void main() {
    outColor = texture(DiffuseMap, TexCoord) * Color;
}