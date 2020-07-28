#version 460
in vec3 pos;
in vec2 texcoord;
in vec3 normal;
in vec3 offset;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMat;

out vec3 V_normal;
out vec4 V_pos;
out vec2 V_texcoord;

void main()
{
	V_normal = mat3(NormalMat)*normal;
	V_pos = M*vec4(pos, 1.0);
	V_texcoord = texcoord;
    gl_Position = P*V*M*vec4(pos, 1.0);
}