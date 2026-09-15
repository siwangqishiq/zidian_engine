#version 450

// 接收顶点着色器插值后的颜色
layout(location = 0) in vec4 fragColor;
layout(location = 1) in float radius;

// 输出颜色
layout(location = 0) out vec4 outColor;

void main(){
    vec2 p = gl_PointCoord;
    vec2 c = vec2(0.5f, 0.5f);

    if(length(p - c) > 0.5f){
        discard;
    }
    outColor = fragColor;
}