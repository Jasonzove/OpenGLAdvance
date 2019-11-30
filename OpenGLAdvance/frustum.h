#pragma once
#include "glew.h"

class Frustum
{
public:

public:
	GLuint mProgram;
	GLuint mVbo;
	GLuint mIbo;
	GLint mPosLocation;
	GLint mPLocation;
	GLint mVLocation;
	GLint mMLocation;

public:
	void InitProgram();
	void InitPerspective(const float& fov, const float& aspect, const float& zNear, const float& zFar);
	void InitOrtho(const float& left, const float& right, const  float bottom, const float& top, const float& zNear, const float& zFar);
	void Draw(const float* const& M, const float* const& V, const float* const& P);
};