#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in uint inMatIndex;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

layout(set = 0, binding = 0, std430) readonly buffer TransformBuffer {
    mat4 transforms[];
};

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = pc.proj * vec4(inPosition , 1.0f);
    fragColor = inColor;
}
