#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec4 inColor;

// layout(binding = 0) uniform UB {
//     mat4 proj;
// } ub;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 color;

void main() {
    gl_Position = pc.proj * vec4(inPosition , 1.0f);
    uv = inUv;
    color = inColor;
}
