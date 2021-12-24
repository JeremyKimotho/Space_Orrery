#version 330 core

in vec3 fragPos;
in vec3 fragColor;
in vec3 n;
in vec2 tc;

uniform sampler2D sampler;
uniform vec3 light;

out vec4 color;

void main() {
    vec3 tex = texture(sampler, tc).xyz;
	vec3 lightDir = normalize(light - fragPos);
    vec3 normal = normalize(n);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = 0.12 * tex;
    vec3 diffuse = diff * tex;

    color = vec4((ambient + diffuse), 1.0);

}
