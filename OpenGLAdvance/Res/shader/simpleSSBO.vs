#version 460 //430之后的版本才支持SSBO

//SSBO只支持vec4
struct Vertex
{
	vec4 pos;
	vec4 texcoord;
	vec4 normal;
};

//std140,std430两个版本
layout(std430, binding=0)buffer Mesh{
	Vertex vertexes[];
}mesh;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 V_texcoord;

void main()
{
	V_texcoord = mesh.vertexes[gl_VertexID].texcoord.xy;

	gl_Position = P*V*M*vec4(mesh.vertexes[gl_VertexID].pos.xyz, 1.0);
}