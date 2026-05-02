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
    // 바닥은 하얀색에 가까운 밝은 회색
    vec3 objectColor = vec3(0.8, 0.8, 0.8); 

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_LightColor;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    float specularStrength = 0.3;
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); 
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0);
    color2 = u_EntityID;
}
