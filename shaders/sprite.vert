#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUv;

layout(binding = 0) uniform UB {
    mat4 proj;
} ub;


layout(location = 0) out vec2 uv;

void main() {
    gl_Position = ub.proj * vec4(inPosition , 1.0f);
    uv = inUv;
}
