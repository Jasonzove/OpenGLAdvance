#version 460
in vec3 pos;
in vec2 texcoord;
in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMat;

void main()
{
	gl_PointSize = 40.0f;
    gl_Position = P*V*M*vec4(pos, 1.0);
}