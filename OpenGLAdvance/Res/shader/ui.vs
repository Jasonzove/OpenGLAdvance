#version 460
in vec3 pos;
in vec2 texcoord;
in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NormalMat;

out vec2 V_texcoord;

void main()
{
	V_texcoord = vec2(texcoord.x, 1.0-texcoord.y);
    gl_Position = P*V*M*vec4(pos, 1.0);
}