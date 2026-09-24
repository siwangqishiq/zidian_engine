#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inRadius;
layout(location = 2) in vec4 inColor;

// layout(binding = 0) uniform UB {
//     mat4 proj;
// } ub;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out float radius;


void main() {
    gl_Position = pc.proj * vec4(inPosition , 1.0f);
    gl_PointSize = inRadius * 2.0f;

    fragColor = inColor;
    radius = inRadius;
}
