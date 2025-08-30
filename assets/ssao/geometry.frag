#version 460 core

out vec4 _Color;

uniform int type;

in vec2 _TexCoords;

const int GRID_NUM = 5;

void main() {
    if(type < 0) {
        discard;
    }

    if(_TexCoords.x > 0.01 && _TexCoords.x < 0.99 && _TexCoords.y > 0.01 && _TexCoords.y < 0.99) {
        vec2 tex = float(GRID_NUM) * _TexCoords;
        int check = (int(tex.x) + int(tex.y)) % 2;
        if(check == 0) {
            _Color = vec4(vec3(0.3), 1.0);
        } else {
            _Color = vec4(vec3(0.7), 1.0);
        }
    } else {
        _Color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
