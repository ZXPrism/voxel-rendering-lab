#version 460 core

layout(location = 0) in vec3 _Pos;
layout(location = 1) in vec2 _UV;

out vec2 inter_uv;

void main() {
    inter_uv = _UV;
    gl_Position = vec4(_Pos, 1.0);
}
