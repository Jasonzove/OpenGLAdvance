#include "tool.h"
#include <stdio.h>

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

	//����GPU
	glShaderSource(shader, 1, &shaderCode, nullptr);
	//����
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

	//����program��attach
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

	int imageWidth;
	int imageHeight;
	unsigned char* pPixelData = DecodeBMP(pFileContent, imageWidth, imageHeight);
	if (pPixelData == nullptr)
	{
		printf("decode file content failed!\n");
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pPixelData);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete pFileContent;
	pFileContent = nullptr;

	return texture;
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

	if (*((unsigned short*)fileContent) != 0x4D42) //����bmpͼ��
	{
		printf("current file is not BMP!\n");
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