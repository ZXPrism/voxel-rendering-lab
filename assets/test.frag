#version 460 core

in vec2 uv;

out vec4 _Color;

void main() {
    vec2 uv_mapped = uv - vec2(0.5);

    if(max(abs(uv_mapped.x), abs(uv_mapped.y)) < 0.45) {
        _Color = vec4(0.95, 0.35, 0.35, 1.0);
    } else {
        _Color = vec4(vec3(0.0), 1.0);
    }
}
