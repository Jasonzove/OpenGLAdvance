#include "tool.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>

/*****************OpenGL***********************/
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void* data /* = nullptr */)
{
	GLuint object;
	glGenBuffers(1, &object);
	glBindBuffer(bufferType, object);
	glBufferData(bufferType, size, data, usage);
	glBindBuffer(bufferType, 0);
	return object;
}

GLuint CompileShader(GLenum shaderType, const char* const& shaderPath)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		printf("glCreateShader failed!\n");
		return 0;
	}

	const char* shaderCode = LoadFileContent(shaderPath);
	if (shaderCode == nullptr)
	{
		printf("load shader code from file failed!: %s\n", shaderPath);
		glDeleteShader(shader);
		return 0;
	}

	//传入GPU
	glShaderSource(shader, 1, &shaderCode, nullptr);
	//编译
	glCompileShader(shader);
	GLint compileResult = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult != GL_TRUE)
	{
		char szLog[1024] = { 0 };
		GLsizei logLen = 0;
		glGetShaderInfoLog(shader, 1024, &logLen, szLog);
		printf("compile shader failed!: %s\nshader code:\n%s\n", szLog, shaderCode);
		glDeleteShader(shader);
		shader = 0;
	}

	delete shaderCode;
	shaderCode = nullptr;

	return shader;
}

GLuint CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath)
{
	GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsShaderPath);
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsShaderPath);

	//创建program并attach
	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	//Link
	glLinkProgram(program);
	//detach and delete
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
		printf("link shader failed!: %s\nshader path:\n vs: %s\n fs:%s\n", szLog, vsShaderPath, fsShaderPath);
		glDeleteProgram(program);
		program = 0;
	}

	return program;
}

GLuint CreateTextureFromFile(const char* const& imagePath)
{
	char* pFileContent = LoadFileContent(imagePath);
	if (pFileContent == nullptr)
	{
		printf("load file content failed!\n");
		return 0;
	}

	int imageWidth = 0;
	int imageHeight = 0;
	int pixelDataSize = 0;
	GLenum srcFormat = GL_RGB;
	unsigned char* pPixelData = nullptr;

	if (*((unsigned short*)pFileContent) == 0x4D42) //不是bmp图像
	{
		pPixelData = DecodeBMP(pFileContent, imageWidth, imageHeight);
	}
	else if(memcmp(pFileContent, "DDS ", 4) == 0)
	{
		pPixelData = DecodeDXTData(pFileContent, imageWidth, imageHeight, pixelDataSize);
		srcFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	}

	if (pPixelData == nullptr)
	{
		printf("decode file content failed!\n");
		delete pFileContent;
		pFileContent = nullptr;
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	switch (srcFormat)
	{
	case GL_RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, srcFormat, imageWidth, imageHeight, 0, srcFormat, GL_UNSIGNED_BYTE, pPixelData);
		break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, srcFormat, imageWidth, imageHeight, 0, pixelDataSize, pPixelData);
		break;
	default:
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	//SavePixelDataToBMP("Res/image/test_BMP.bmp", pPixelData, imageWidth, imageHeight);

	delete pFileContent;
	pFileContent = nullptr;

	return texture;
}

void CheckGLError(const char* const& file, const int& line)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_ENUM:
			printf("error: GL_INVALID_ENUM! file: %s, line: %d\n", file, line);
			break;
		case GL_INVALID_VALUE:
			printf("error: GL_INVALID_VALUE! file: %s, line: %d\n", file, line);
			break;
		case GL_INVALID_OPERATION:
			printf("error: GL_INVALID_ENUM! file: %s, line: %d\n", file, line);
			break;
		case GL_STACK_OVERFLOW:
			printf("error: GL_INVALID_OPERATION! file: %s, line: %d\n", file, line);
			break;
		case GL_STACK_UNDERFLOW:
			printf("error: GL_STACK_UNDERFLOW! file: %s, line: %d\n", file, line);
			break;
		case GL_OUT_OF_MEMORY:
			printf("error: GL_OUT_OF_MEMORY! file: %s, line: %d\n", file, line);
			break;
		default:
			break;
		}
	}
}

GLuint CreatVAOWithVBO(std::function<void()> settings)
{
	GLuint vao; //vao不能替代vbo，只是对其的封装
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	settings();
	glBindVertexArray(0);

	return vao;
}



/*****************File***********************/
char* LoadFileContent(const char* path)
{
	FILE* pFile = fopen(path, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int nlen = ftell(pFile);
		char* buffer = new char[nlen + 1];
		rewind(pFile);
		fread(buffer, nlen, 1, pFile);
		buffer[nlen] = '\0';
		fclose(pFile);
		return buffer;
	}
	fclose(pFile);
	return nullptr;
}

unsigned char* DecodeBMP(const char* const& fileContent, int& width, int& height)
{
	if (fileContent == nullptr)
	{
		printf("filecontent is nullptr!\n");
		return nullptr;
	}

	unsigned char* pixelData = nullptr;
	int pixelDataOffset = *((int*)(fileContent + 10));
	width = *((int*)(fileContent + 18));
	height = *((int*)(fileContent + 22));
	pixelData = (unsigned char*)fileContent + pixelDataOffset;
	if (pixelData == nullptr)
	{
		printf("pixel data is null\n");
		return nullptr;
	}

	//bgr -> rgb
	for (int i = 0; i < width*height*3; i += 3)
	{
		pixelData[i] = pixelData[i] ^ pixelData[i + 2];
		pixelData[i+2] = pixelData[i] ^ pixelData[i + 2];
		pixelData[i] = pixelData[i] ^ pixelData[i + 2];
	}

	return pixelData;
}

const unsigned long FORMAT_DTX1 = 0x31545844l; //DTX1:1->X->T->D
unsigned char* DecodeDXTData(const char* const& fileContent, int& width, int& height, int& pixelDataSize)
{
	height = *((unsigned long*)(fileContent + sizeof(unsigned long) * 3));
	width = *((unsigned long*)(fileContent + sizeof(unsigned long) * 4));
	pixelDataSize = *((unsigned long*)(fileContent + sizeof(unsigned long) * 5));
	unsigned long compressFromat;
	compressFromat = *((unsigned long*)(fileContent + sizeof(unsigned long) * 21));
	switch (compressFromat)
	{
	case FORMAT_DTX1:
		printf("DTX1\n");
		break;
	default:
		break;
	}

	unsigned char* pixelData = new unsigned char[pixelDataSize];
	memcpy(pixelData, fileContent + sizeof(unsigned long) * 32, pixelDataSize);

	return pixelData;
}

void SavePixelDataToBMP(const char* const& filePath, unsigned char* const& pixelData, const int& width, const int& height)
{
	FILE* pFile = fopen(filePath, "wb");
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

		unsigned char temp = 0;
		for (int i = 0; i < width*height*3; i += 3)
		{
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i + 2] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
		}
		fwrite(pixelData, width*height * 3, 1, pFile);
	}

	fclose(pFile);
	return;
}