attribute vec3 pos;
attribute vec2 texcoord;
attribute vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMatrix;

varying vec3 V_Normal;

void main()
{
	V_Normal = mat3(NormalMatrix)*normal;
    gl_Position = P*V*M*vec4(pos, 1.0);
}