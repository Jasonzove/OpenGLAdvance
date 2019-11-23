#include "model.h"
#include "tool.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>


VertexData* LoadObjModel(const char* const& filePath, unsigned int** indexes, int& vertexCount, int& indexCount)
{
	char* fileContent = LoadFileContent(filePath);
	if (fileContent != nullptr)
	{
		//decode obj model
		std::stringstream ssObjFile(fileContent);
		char szOneLine[256];//一行不超过256
		std::string temp;

		std::vector<VertexInfo> positions;
		std::vector<VertexInfo> textcoords;
		std::vector<VertexInfo> normals;

		while (!ssObjFile.eof()) //是否最后一行
		{
			memset(szOneLine, 0, 256);
			ssObjFile.getline(szOneLine, 256);
			if (strlen(szOneLine) > 0) //过滤空行
			{
				std::stringstream ssOneLine(szOneLine);

				if (szOneLine[0] == 'v')
				{
					if (szOneLine[1] == 't')
					{
						//textcoord
						ssOneLine >> temp; //输出'vt'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						textcoords.push_back(vi);
						printf("textcoord:%f, %f\n", vi.v[0], vi.v[1]);
					}
					else if (szOneLine[1] == 'n')
					{
						//normal
						ssOneLine >> temp; //输出'vn'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						ssOneLine >> vi.v[2];
						normals.push_back(vi);
						printf("normal:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
					}
					else
					{
						//position
						ssOneLine >> temp; //输出'v'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						ssOneLine >> vi.v[2];
						positions.push_back(vi);
						printf("position:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
					}
				}
				else if (szOneLine[0] == 'f')
				{
					//face
					printf("face:%s\n", szOneLine);

				}
			}
		}

	}

	return nullptr;
}