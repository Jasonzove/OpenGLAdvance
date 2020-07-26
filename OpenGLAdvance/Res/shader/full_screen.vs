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
	V_texcoord = texcoord;
	vec4 position = vec4(pos.x*2,pos.y*2,pos.z, 1.0); //quad.obj�����귶ΧΪ-0.5~0.5,����ֱ��ӳ�䵽ndc��-1~1
    gl_Position = position;
}