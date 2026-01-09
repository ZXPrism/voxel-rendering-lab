#version 460 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vUv;

out vec4 FragColor;

uniform vec3 uCameraPos;

const float AMBIENT_COEFF = 0.05;
const float DIFFUSE_COEFF = 0.7;
const float SPECULAR_COEFF = 0.2;
const int SHININESS = 64;

const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0);
const vec3 LIGHT_POS = vec3(1.0, 0.0, 0.0);

void main() {
    vec3 albedo = (vUv.x >= 0.05 && vUv.x <= 0.95 && vUv.y >= 0.05 && vUv.y <= 0.95) ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 0.0, 0.0);

    // ambient
    vec3 ambient = LIGHT_COLOR * albedo;

    // diffuse
    vec3 light = normalize(LIGHT_POS - vPos);
    vec3 diffuse = LIGHT_COLOR * albedo * max(0, dot(vNormal, light));

    // specular
    vec3 view = normalize(uCameraPos - vPos);
    vec3 half_vec = normalize(light + view);
    vec3 specular = LIGHT_COLOR * pow(max(0, dot(half_vec, vNormal)), SHININESS);

    FragColor = vec4(AMBIENT_COEFF * ambient + DIFFUSE_COEFF * diffuse + SPECULAR_COEFF * specular, 1.0);
}
