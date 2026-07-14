#version 450

// 接收顶点着色器插值后的颜色
layout(location = 0) in vec4 fragColor;

// 输出颜色
layout(location = 0) out vec4 outColor;

void main(){
    outColor = fragColor;
}