#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    // Vulkan坐标系
    gl_Position = vec4(inPosition, 1.0f);
    fragColor = inColor;
}