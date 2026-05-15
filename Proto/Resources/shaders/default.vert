#version 330 core
layout(location=0) in vec3 a_Position;
layout(location=1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_FragPos;
out vec3 v_Normal;

void main() {
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}
