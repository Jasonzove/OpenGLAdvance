#include <windows.h>
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

GLuint CreateGPUProgram(const char * const & vsShaderCode, const char * const & fsShaderCode)
{
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vsShader, 1, &vsShaderCode, nullptr); //´«ÈëÏÔ¿¨
	glShaderSource(fsShader, 1, &fsShaderCode, nullptr);
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

unsigned char* LoadBMP(const char* path, int& width, int& height)
{
	unsigned char* imageData = nullptr;
	FILE* pFile;
	fopen_s(&pFile, path, "rb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		fread(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);
		if (bfh.bfType == 0x4D42)
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
				temp = imageData[i + 2];
				imageData[i + 2] = imageData[i];
				imageData[i] = temp;
			}
		}
		fclose(pFile);
	}
	return imageData;
}

