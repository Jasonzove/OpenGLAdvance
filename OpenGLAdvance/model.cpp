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
		char szOneLine[256];//һ�в�����256
		std::string temp;

		std::vector<VertexInfo> positions;
		std::vector<VertexInfo> textcoords;
		std::vector<VertexInfo> normals;

		std::vector<unsigned int> objIndexes; //�������
		std::vector<VertexDefine> vertices; //face�е����е�

		while (!ssObjFile.eof()) //�Ƿ����һ��
		{
			memset(szOneLine, 0, 256);
			ssObjFile.getline(szOneLine, 256);
			if (strlen(szOneLine) > 0) //���˿���
			{
				std::stringstream ssOneLine(szOneLine);

				if (szOneLine[0] == 'v')
				{
					if (szOneLine[1] == 't')
					{
						//textcoord
						ssOneLine >> temp; //���'vt'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						textcoords.push_back(vi);
						//printf("textcoord:%f, %f\n", vi.v[0], vi.v[1]);
					}
					else if (szOneLine[1] == 'n')
					{
						//normal
						ssOneLine >> temp; //���'vn'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						ssOneLine >> vi.v[2];
						normals.push_back(vi);
						//printf("normal:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
					}
					else
					{
						//position
						ssOneLine >> temp; //���'v'
						VertexInfo vi;
						ssOneLine >> vi.v[0];
						ssOneLine >> vi.v[1];
						ssOneLine >> vi.v[2];
						positions.push_back(vi);
						//printf("position:%f, %f, %f\n", vi.v[0], vi.v[1], vi.v[2]);
					}
				}
				else if (szOneLine[0] == 'f')
				{
					//face
					ssOneLine >> temp; //���'f'
					std::string vertexStr;
					for (int i = 0; i < 3; ++i)
					{
						ssOneLine >> vertexStr;
						size_t pos = vertexStr.find_first_of('/');
						std::string positionIndexStr = vertexStr.substr(0, pos);
						size_t pos2 = vertexStr.find_first_of('/', pos+1);
						std::string texcoordIndexStr = vertexStr.substr(pos + 1, pos2 - (pos + 1));
						std::string normalIndexStr = vertexStr.substr(pos2 + 1, vertexStr.length()- (pos2+1));

						VertexDefine vd;
						vd.positionIndex = atoi(positionIndexStr.c_str()) - 1;//��1/1/1,��1��ʼ�����������±��0��ʼ
						vd.texcoordIndex = atoi(texcoordIndexStr.c_str()) - 1;
						vd.normalIndex = atoi(normalIndexStr.c_str()) - 1;

						//ȥ��face�е��ظ��㣬���������
						int nCurrentIndex = -1;
						for (int j = 0; j < vertices.size(); ++j)
						{
							if (vd.positionIndex == vertices[j].positionIndex &&
								vd.texcoordIndex == vertices[j].texcoordIndex &&
								vd.normalIndex == vertices[j].normalIndex)
							{
								nCurrentIndex = j;
								break;
							}
						}

						//û���ظ�
						if (nCurrentIndex == -1)
						{
							vertices.push_back(vd); //�������� -> vbo
							nCurrentIndex = vertices.size() - 1; //�����±������
						}

						objIndexes.push_back(nCurrentIndex); //�������� -> ibo
					}
				}
			}
		}
		//printf("%u\n", objIndexes.size() / 3);
		//objIndexes -> index buffer -> ibo
		indexCount = static_cast<unsigned int>(objIndexes.size());
		*indexes = new unsigned int[indexCount];
		memcpy(*indexes, &objIndexes[0], indexCount * sizeof(unsigned int));

		//vertices -> vertexes -> vbo
		vertexCount = vertices.size();
		VertexData* vertexes = new VertexData[vertexCount];
		for (int k = 0; k < vertexCount; ++k)
		{
			memcpy(vertexes[k].position, positions[vertices[k].positionIndex].v, sizeof(float) * 3);
			memcpy(vertexes[k].textcoord, textcoords[vertices[k].texcoordIndex].v, sizeof(float) * 2);
			memcpy(vertexes[k].normal, normals[vertices[k].normalIndex].v, sizeof(float) * 3);
		}

		return vertexes;
	}

	return nullptr;
}