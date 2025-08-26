#version 460 core

in vec2 _TexCoords;
out vec4 _Color;

uniform sampler2D sampler;
uniform int window_width;
uniform int window_height;

float kernel[3][3] = float[3][3]( //
float[3](1.0 / 9, 1.0 / 9, 1.0 / 9), //
float[3](1.0 / 9, 1.0 / 9, 1.0 / 9), //
float[3](1.0 / 9, 1.0 / 9, 1.0 / 9) //
);

void main() {
    vec4 sum = vec4(0.0);

    float dx = 1.0 / window_width;
    float dy = 1.0 / window_height;

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(float(x), float(y)) * vec2(dx, dy);
            sum += texture(sampler, _TexCoords + offset) * kernel[x + 1][y + 1];
        }
    }

    _Color = sum;
}
