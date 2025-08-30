#version 460 core

in vec2 inter_uv;
out vec4 color;

uniform sampler2D sampler_albedo;
uniform sampler2D sampler_position;
uniform sampler2D sampler_normal;

uniform int window_width;
uniform int window_height;

uniform vec3 light_position;
uniform vec3 light_color;

uniform int u_display_mode;

void main() {
    int display_mode = u_display_mode;
    if(u_display_mode == 4) { // all
        display_mode = 0;
        display_mode |= int(int(gl_FragCoord.x) < window_width / 2);
        display_mode |= int(int(gl_FragCoord.y) < window_height / 2) << 1;
    }

    vec3 normal = texture(sampler_normal, inter_uv).xyz;
    if(dot(normal, normal) == 0.0) {
        discard;
    }

    vec3 position = texture(sampler_position, inter_uv).xyz;

    switch(display_mode) {
        case 0: // final
            vec3 light = normalize(light_position - position);
            float diffuse = dot(light, normal);
            color = vec4(diffuse * light_color * texture(sampler_albedo, inter_uv).xyz, 1.0);
            break;
        case 1: // albedo
            color = texture(sampler_albedo, inter_uv);
            break;
        case 2: // position
            color = vec4(normalize(abs(position)), 1.0);
            break;
        case 3: // normal
            color = vec4(abs(normal), 1.0);
            break;
    }
}
