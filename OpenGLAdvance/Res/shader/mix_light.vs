#version 430

in vec3 pos;
in vec2 texcoord;
in vec3 normal;
in vec3 offeset;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMatrix;

out vec3 V_Normal;
out vec4 V_WorldPos;
out vec2 V_Texcoord;

void main()
{
	V_Normal = mat3(NormalMatrix)*normal;
	V_WorldPos = M*vec4(pos, 1.0);
	V_Texcoord = texcoord;
    gl_Position = P*V*V_WorldPos;
}