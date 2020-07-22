#pragma once
#include "glew.h"

char* LoadFileContent(const char* const& filePath);

GLuint CreateGPUProgram(const char* const& vsShaderCode, const char* const& fsShaderCode);

GLuint CreateGPUBufferObject(GLenum targetType, GLsizeiptr size, GLenum usage, const void* data = nullptr);

unsigned char* LoadBMP(const char* path, int& width, int& height);

void SavePixelDataToBMP(
	const char* const& filePath,
	unsigned char* const& pixelData,
	const int& width,
	const int& height);

