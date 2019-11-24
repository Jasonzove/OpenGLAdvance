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