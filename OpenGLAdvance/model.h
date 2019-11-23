#pragma once
struct VertexData
{
	float position[3];
	float textcoord[2];
	float normal[3];
};

struct VertexInfo
{
	float v[3];
};

struct VertexDefine //һ��face�У�ÿ��������ݽṹ
{
	int positionIndex;
	int texcoordIndex;
	int normalIndex;
};

VertexData* LoadObjModel(const char* const& filePath, unsigned int** indexes, int& vertexCount, int& indexCount);