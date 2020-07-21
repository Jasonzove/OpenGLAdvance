#include <stdio.h>
#include "utils.h"

char* LoadFileContent(const char* const& filePath)
{
	if (filePath == nullptr)
	{
		printf("LoadFileContent():filePath is nullptr!\n");
		return nullptr;
	}
	FILE* pFile = nullptr;
	fopen_s(&pFile, filePath, "rb");
	if (pFile == nullptr)
	{
		printf("LoadFileContent():open file failed!\n");
		return nullptr;
	}

	fseek(pFile, 0, SEEK_END);
	int len = ftell(pFile);
	char* buffer = new char[len + 1];
	rewind(pFile);
	fread(buffer, len, 1, pFile);
	buffer[len] = '\0';
	fclose(pFile);

	//printf("%s\n", buffer);

	return buffer;
}

GLuint CreateGPUProgram(const char * const & vsShaderPath, const char * const & fsShaderPath)
{
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vsCode = LoadFileContent(vsShaderPath);
	const char* fsCode = LoadFileContent(fsShaderPath);
	glShaderSource(vsShader, 1, &vsCode, nullptr); //´«ÈëÏÔ¿¨
	glShaderSource(fsShader, 1, &fsCode, nullptr);
	glCompileShader(vsShader);
	glCompileShader(fsShader);
	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	glLinkProgram(program);
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);

	return program;
}

GLuint CreateGPUBufferObject(GLenum targetType, GLsizeiptr size, GLenum usage, const void * data)
{
	GLuint object;
	glGenBuffers(1, &object);
	glBindBuffer(targetType, object);
	glBufferData(targetType, size, data, usage);
	glBindBuffer(targetType, 0);
	return object;
}



