#include <windows.h>
#include <stdio.h>
#include "utils.h"

static GLuint CompileShader(const int& shaderType, const char * const & shaderCode);

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

GLuint CompileShader(const int& shaderType, const char * const & shaderCode)
{
	if (shaderCode == nullptr)
	{
		printf("CompileShader():shaderCode is nullptr!\n");
		return 0;
	}
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		glDeleteShader(shader);
		shader = 0;
		printf("CompileShader():glCreateShader failed!\n");
		return 0;
	}
	glShaderSource(shader, 1, &shaderCode, nullptr); //传入显卡
	glCompileShader(shader);
	GLint compileResult = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult != GL_TRUE)
	{
		char szLog[1024] = { 0 };
		GLsizei logLen = 0;
		glGetShaderInfoLog(shader, 1024, &logLen, szLog);
		printf("CompileShader():glCompileShader failed!\nlog\n:%s\ncode\n:%s\n", szLog, shaderCode);
		glDeleteShader(shader);
		shader = 0;
		return shader;
	}

	return shader;
}

GLuint CreateGPUProgram(const char * const & vsShaderCode, const char * const & fsShaderCode)
{
	GLuint vsShader = CompileShader(GL_VERTEX_SHADER,vsShaderCode);
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsShaderCode);

	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	glLinkProgram(program);
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);

	GLint linkResult = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
	if (linkResult != GL_TRUE)
	{
		char szLog[1024] = { 0 };
		GLsizei logLen = 0;
		glGetProgramInfoLog(program, 1024, &logLen, szLog);
		printf("CreateGPUProgram():glLinkProgram failed!\nvs\n:%s\nfs\n:%s\n", vsShaderCode, fsShaderCode);
		glDeleteProgram(program);
		program = 0;
		return program;
	}

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

GLuint CreateTexture(const char* const& filePath)
{
	if (filePath == nullptr)
	{
		printf("CreateTexture(): filepath is nullpt!\n");
		return -1;
	}
	int width = 0;
	int height = 0;
	unsigned char* piexelData = LoadBMP(filePath, width, height);
	if (nullptr == piexelData)
	{
		printf("CreateTexture(): loadBMP data failed!\n");
		return -1;
	}

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //超过的变成1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, piexelData);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete piexelData;
	piexelData = nullptr;

	return textureId;
}

void SavePixelDataToBMP(
	const char* const& filePath,
	unsigned char* const& pixelData,
	const int& width,
	const int& height)
{
	FILE* pFile;
	fopen_s(&pFile, filePath, "wb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
		bfh.bfType = 0x4D42;
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);

		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(BITMAPINFOHEADER));
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biBitCount = 24;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);

		for (int i = 0; i < width * height * 3; i += 3)
		{
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i + 2] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
		}
		fwrite(pixelData, width * height * 3, 1, pFile);
	}

	fclose(pFile);
	return;
}

unsigned char* LoadBMP(const char* const& path, int& width, int& height)
{
	unsigned char* imageData = nullptr;
	FILE* pFile;
	fopen_s(&pFile, path, "rb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		fread(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);
		if (bfh.bfType == 0x4D42) //0x4D42:bmp
		{
			BITMAPINFOHEADER bih;
			fread(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);
			width = bih.biWidth;
			height = bih.biHeight;
			int pixelCount = width * height * 3;
			imageData = new unsigned char[pixelCount];
			fseek(pFile, bfh.bfOffBits, SEEK_SET);
			fread(imageData, 1, pixelCount, pFile);

			unsigned char temp;
			for (int i = 0; i < pixelCount; i += 3)
			{
				imageData[i] = imageData[i] ^ imageData[i + 2];
				imageData[i + 2] = imageData[i] ^ imageData[i + 2];
				imageData[i] = imageData[i] ^ imageData[i + 2];
			}
		}
		fclose(pFile);
	}
	return imageData;
}

