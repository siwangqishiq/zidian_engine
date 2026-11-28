#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inSize;
layout(location = 2) in vec2 inPa;
layout(location = 3) in vec2 inPb;
layout(location = 4) in vec2 inPc;
layout(location = 5) in vec4 inColor;
layout(location = 6) in uint inMatIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outPa;
layout(location = 2) out vec2 outPb;
layout(location = 3) out vec2 outPc;

layout(set = 0, binding = 0, std430) readonly buffer TransformBuffer {
    mat4 transforms[];
} transBuffer;

layout(push_constant) uniform PushConstant {
    mat4 proj;
} pc;

void main() {
    gl_Position = pc.proj * transBuffer.transforms[inMatIndex] * vec4(inPosition , 1.0f);
    gl_PointSize = inSize;

    fragColor = inColor;
    
    outPa = inPa;
    outPb = inPb;
    outPc = inPc;
}
