#version 450

// 接收顶点着色器插值后的颜色
layout(location = 0) in vec4 fragColor;

layout(location = 1) in vec2 outPa;
layout(location = 2) in vec2 outPb;
layout(location = 3) in vec2 outPc;

// 输出颜色
layout(location = 0) out vec4 OutColor;

// 判断点p 在直线ab的左侧 右侧 还是直线上
float edge(vec2 a, vec2 b, vec2 p) {
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

void main(){
    vec2 p = gl_PointCoord;
    // 等边三角形三个顶点
    vec2 A = outPa;
    vec2 B = outPb;
    vec2 C = outPc;
    
    float e1 = edge(A, B, p);
    float e2 = edge(B, C, p);
    float e3 = edge(C, A, p);

    if (e1 < 0.0f || e2 < 0.0f || e3 < 0.0f) {
        discard;
    }
    OutColor = fragColor;
}