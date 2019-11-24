#pragma once
#include "glew.h"

/***********OpenGL***************/
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void* data = nullptr);

GLuint CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath);

GLuint CompileShader(GLenum shaderType, const char* const& shaderPath);










/***********File***************/
char* LoadFileContent(const char* path);
