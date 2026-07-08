#version 450

// 输入
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

// 输出到 Fragment Shader
layout(location = 0) out vec3 fragColor;

void main() {
    // Vulkan坐标系
    gl_Position = vec4(inPosition, 0.0f, 1.0f);

    fragColor = inColor;
}