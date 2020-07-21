#pragma once
#include "vertexdata.h"

class ObjModel
{
public:
	VertexData* LoadObjModel(const char* const& pFilePath, int** pIndexes, int& vetexCount, int& indexCount);

	//void Draw();

public:
	//VertexData* mVertexes;
	//int* mIndices;
	//int mIndexCount;
};
