#version 460 core

uniform int type;

in vec2 uv;
out vec4 _Color;

const vec3 map_type_to_color[] = vec3[]( //
vec3(1.0, 0.0, 0.0), // 0
vec3(0.0, 1.0, 0.0), // 1
vec3(0.0, 0.0, 1.0) // 2
);

void main() {
    if(type < 0) {
        discard;
    }

    vec2 uv_mapped = uv - vec2(0.5);

    if(max(abs(uv_mapped.x), abs(uv_mapped.y)) < 0.45) {
        _Color = vec4(map_type_to_color[type], 1.0);
    } else {
        _Color = vec4(vec3(0.0), 1.0);
    }
}
