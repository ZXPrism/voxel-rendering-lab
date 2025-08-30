#version 460 core

layout(location = 0) in vec3 _Pos;
layout(location = 1) in vec3 _Normal;
layout(location = 2) in vec2 _UV;

uniform mat4 model;
uniform mat4 model_normal;
uniform mat4 view;
uniform mat4 projection;

out vec2 inter_uv;
out vec3 inter_normal;
out vec3 inter_position;

void main() {
    inter_uv = _UV;

    vec4 new_normal = model_normal * vec4(_Normal, 0.0);
    inter_normal = normalize(new_normal.xyz);

    vec4 position = model * vec4(_Pos, 1.0);
    inter_position = position.xyz / position.w;

    gl_Position = projection * view * model * vec4(_Pos, 1.0);

}
