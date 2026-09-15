#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inSize;
layout(location = 2) in vec4 inColor;

layout(binding = 0) uniform UB {
    mat4 proj;
} ub;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out float radius;


void main() {
    gl_Position = ub.proj * vec4(inPosition , 1.0f);
    const float size = inSize;
    gl_PointSize = size;

    fragColor = inColor;
    radius = size / 2.0f;
}
