#pragma once
#include "vertexdata.h"

class ObjModel
{
public:
	VertexData* LoadObjModel(const char* const& pFilePath, int** pIndexes, int& vetexCount, int& indexCount);

public:
	VertexData* mVertexes;
	int* mIndices;
	int mIndexCount;
};
