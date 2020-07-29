#pragma once
#include <functional>
#include "glew.h"

char* LoadFileContent(const char* const& filePath);

GLuint CreateGPUProgram(const char* const& vsShaderCode, const char* const& fsShaderCode);

GLuint CreateGPUBufferObject(GLenum targetType, GLsizeiptr size, GLenum usage, const void* data = nullptr);

GLuint CreatVAO(std::function<void()> setting);

GLuint CreateTexture(const char* const& filePath);

unsigned char* LoadBMP(const char* const& path, int& width, int& height);

void SavePixelDataToBMP(
	const char* const& filePath,
	unsigned char* const& pixelData,
	const int& width,
	const int& height);

void CheckGLError(const char*const& pFile, const int& line);
#define GL_CHECK(x) \
do\
{\
	x;\
	CheckGLError(__FILE__, __LINE__);\
} while (false);

