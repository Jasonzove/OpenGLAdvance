#pragma once
#include "glew.h"

char* LoadFileContent(const char* const& filePath);

GLuint CreateGPUProgram(const char* const& vsShaderPath, const char* const& fsShaderPath);

GLuint CreateGPUBufferObject(GLenum targetType, GLsizeiptr size, GLenum usage, const void* data = nullptr);