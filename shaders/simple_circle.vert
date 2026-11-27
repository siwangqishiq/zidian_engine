#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inRadius;
layout(location = 2) in vec4 inColor;
layout(location = 3) in uint inMatIndex;

layout(set = 0, binding = 0, std430) readonly buffer TransformBuffer {
    mat4 transforms[];
} transBuffer;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out float radius;


void main() {
    gl_Position = pc.proj * transBuffer.transforms[inMatIndex] * vec4(inPosition , 1.0f);
    gl_PointSize = inRadius * 2.0f;

    fragColor = inColor;
    radius = inRadius;
}
