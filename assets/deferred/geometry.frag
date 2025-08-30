#version 460 core

in vec2 inter_uv;
in vec3 inter_normal;
in vec3 inter_position;

layout(location = 0) out vec3 rt_albedo;
layout(location = 1) out vec3 rt_position;
layout(location = 2) out vec3 rt_normal;

uniform int type;

const int GRID_NUM = 5;

void main() {
    if(type < 0) {
        discard;
    }

    if(inter_uv.x > 0.01 && inter_uv.x < 0.99 && inter_uv.y > 0.01 && inter_uv.y < 0.99) {
        vec2 tex = float(GRID_NUM) * inter_uv;
        int check = (int(tex.x) + int(tex.y)) % 2;
        if(check == 0) {
            rt_albedo = vec3(0.3);
        } else {
            rt_albedo = vec3(0.7);
        }
    } else {
        rt_albedo = vec3(0.0);
    }

    rt_normal = inter_normal;
    rt_position = inter_position;
}
