#version 460 core

out vec4 _Color;

uniform int type;

in vec2 _TexCoords;

void main() {
    if(type < 0) {
        discard;
    }

    if(_TexCoords.x > 0.05 && _TexCoords.x < 0.95 && _TexCoords.y > 0.05 && _TexCoords.y < 0.95) {
        _Color = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        _Color = vec4(0.0, 0.0, 1.0, 1.0);
    }
}
