#version 430

in vec3 pos;
in vec2 texcoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 V_Texcoord;

void main()
{
	V_Texcoord = vec2(texcoord.x, 1-texcoord.y);
    gl_Position = P*V*M*vec4(pos, 1.0);
}