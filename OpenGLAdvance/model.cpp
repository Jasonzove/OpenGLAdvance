#include "model.h"
#include "tool.h"
#include <stdio.h>
#include <string>
#include <sstream>


VertexData* LoadObjModel(const char* const& filePath, unsigned int** indexes, int& vertexCount, int& indexCount)
{
	char* fileContent = LoadFileContent(filePath);
	if (fileContent != nullptr)
	{
		//decode obj model
		std::stringstream ssObjFile(fileContent);
		char szOneLine[256];//一行不超过256
		while (!ssObjFile.eof()) //是否最后一行
		{
			memset(szOneLine, 0, 256);
			ssObjFile.getline(szOneLine, 256);
			if (strlen(szOneLine) > 0) //过滤空行
			{
				if (szOneLine[0] == 'v')
				{
					if (szOneLine[1] == 't')
					{
						//textcoord
						printf("textcoord:%s\n", szOneLine);
					}
					else if (szOneLine[1] == 'n')
					{
						//normal
						printf("normal:%s\n", szOneLine);
					}
					else
					{
						//position
						printf("position:%s\n", szOneLine);


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