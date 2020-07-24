#version 430
in vec3 pos;
in vec2 texcoord;
in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMat;

out vec3 V_normal;
out vec4 V_pos;

void main()
{
	V_normal = mat3(NormalMat)*normal;
	V_pos = M*vec4(pos, 1.0);
    gl_Position = P*V*M*vec4(pos, 1.0);
}