#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(binding = 0) uniform UB {
    int type;
} ub;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

layout(location = 0) out vec4 fragColor;

void main() {
    // gl_Position = vec4(pc.proj * inPosition, 1.0f);
    gl_Position = pc.proj * vec4(inPosition , 1.0f);
    // gl_Position = vec4(inPosition, 1.0f);
    fragColor = inColor;
}
