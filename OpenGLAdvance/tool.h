#pragma once
#include "glew.h"

/***********OpenGL***************/
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void* data = nullptr);














/***********File***************/
char* LoadFileContent(const char* path);
