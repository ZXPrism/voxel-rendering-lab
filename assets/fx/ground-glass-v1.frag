#version 460 core

in vec2 _TexCoords;
out vec4 _Color;

uniform sampler2D sampler;
uniform int window_width;
uniform int window_height;

const int SAMPLE_NUM = 64;
const float SAMPLE_CIRCLE_RADIUS = 16.0; // in pixels

// simple LCG
float rand(inout float seed) {
    seed = fract(seed * 114514.1919810 + 1234.56789);
    return seed;
}

// randomly sample within a unit circle
vec2 sample_circle(inout float seed) {
    float u = rand(seed);
    float v = rand(seed);
    float r = sqrt(u);
    float theta = 2.0 * 3.1415926 * v;
    return vec2(r * cos(theta), r * sin(theta));
}

void main() {
    vec2 pixel_size = vec2(1.0 / float(window_width), 1.0 / float(window_height));
    float seed = 123.456;

    vec4 sum = vec4(0.0);
    for(int _ = 0; _ < SAMPLE_NUM; _++) {
        vec2 offset = sample_circle(seed) * pixel_size * SAMPLE_CIRCLE_RADIUS;
        sum += texture(sampler, _TexCoords + offset);
    }

    _Color = sum / SAMPLE_NUM;
}
