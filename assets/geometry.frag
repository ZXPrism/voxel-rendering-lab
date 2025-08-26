#version 460 core

out vec4 _Color;

uniform int type;

in vec2 _TexCoords;

const int GRID_NUM = 5;

void main() {
    if(type < 0) {
        discard;
    }

    if(_TexCoords.x > 0.03 && _TexCoords.x < 0.97 && _TexCoords.y > 0.03 && _TexCoords.y < 0.97) {
        vec2 tex = float(GRID_NUM) * _TexCoords;
        _Color = vec4(vec3((int(tex.x) + int(tex.y)) % 2), 1.0);
    } else {
        _Color = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
