#version 430

in vec3 pos;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
	gl_PointSize = 30.0;
	gl_Position = P*V*M*vec4(pos, 1.0);
}