#pragma once
#include "glew.h"
#include "Glm/glm.hpp"
#include "Glm/ext.hpp"

class Frustum
{
public:
	void InitProgram();
	void InitPerspective(const float& fov, const float& aspect, const float& near, const float& far);
	//正交投影有可能是不对称的
	void InitOrtho(const float& left, const float& right, const float& top, const float& bottom, const float& near, const float& far);
	void Draw(const float* const& m, const float* const& v, const float* const& p);

public:
	GLuint mVBO, mEBO;
	GLint mProgram, mPosLocation, mMLocation, mVLocation, mPLocation;
};