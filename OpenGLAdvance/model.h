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

VertexData* LoadObjModel(const char* const& filePath, unsigned int** indexes, int& vertexCount, int& indexCount);