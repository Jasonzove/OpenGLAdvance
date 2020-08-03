#version 460 //430֮��İ汾��֧��SSBO

//SSBOֻ֧��vec4
struct Vertex
{
	vec4 pos;
	vec4 texcoord;
	vec4 normal;
};

//std140,std430�����汾
layout(std430, binding=0)buffer Mesh{
	Vertex vertexes[];
}mesh;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 V_texcoord;

void main()
{
	//�˴������Ǻܶ����Ժ��Լ��о�
	int spriteID = gl_VertexID >> 2;
	vec4 spritePos = vec4(mesh.vertexes[spriteID].pos.xyz, 1.0);
	vec4 spritePosInView = V*M*spritePos;
	vec2 texcoord = vec2(((gl_VertexID - 1)&2)>>1, (gl_VertexID&2)>>1);
	float spriteSize = 0.05;
	vec4 fixedPos = spritePosInView + vec4(vec2(texcoord.x*2.0 - 1.0, texcoord.y*2.0 - 1.0)*spriteSize, 0.0, 1.0);

	V_texcoord = texcoord;

	gl_Position = P*fixedPos;
}