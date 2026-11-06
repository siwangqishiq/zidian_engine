#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 OutColor;

void main(){
    const int gridCount = 10;
    vec2 st = uv;
    st = gridCount * st;
    // st = fract(st);
    ivec2 xy = ivec2(floor(st));
    vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if(xy % 2 == ivec2(0,1) || xy % 2 == ivec2(1,0)){
        color = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    }
    OutColor = color;
}
