#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec3 v_FragPos;
in vec3 v_Normal;

uniform int u_EntityID;
uniform vec3 u_ViewPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;

void main() {
    // 기본 오브젝트 색상 (밝은 회색)
    vec3 objectColor = vec3(0.8, 0.8, 0.8); 

    // 1. Ambient (주변광)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_LightColor;

    // 2. Diffuse (난반사광)
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDir); // 빛이 향하는 방향의 반대 방향 계산
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // 3. Specular (정반사광)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0);
    color2 = u_EntityID;
}
