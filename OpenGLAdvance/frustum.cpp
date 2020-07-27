#include <math.h>
#include "frustum.h"
#include "utils.h"
#include "resource.h"
#include "shader_coder.h"

const float PI = 3.1415926;

void Frustum::InitProgram()
{
	mProgram= CreateGPUProgram(ShaderCoder::Get(IDR_SHADER_white_vs).c_str(),
		ShaderCoder::Get(IDR_SHADER_white_fs).c_str());
	mPosLocation = glGetAttribLocation(mProgram, "pos");
	mMLocation = glGetUniformLocation(mProgram, "M");
	mVLocation = glGetUniformLocation(mProgram, "V");
	mPLocation = glGetUniformLocation(mProgram, "P");
}

void Frustum::InitPerspective(const float & fov, const float & aspect, const float & near, const float & far)
{
	//near
	float zNear = near;
	float halfFov = fov / 2.0f;
	float radianHalfFov = (halfFov / 180.0f)*PI;
	float yNear = tan(radianHalfFov)*zNear;
	float xNear = aspect * yNear;
	//far
	float zFar = far;
	float yFar = tan(radianHalfFov)*zFar;
	float xFar = aspect * yFar;
	//生成点,用于生成VBO
	float vertexes[] = {
		-xNear, -yNear, -zNear,
		xNear, -yNear, -zNear,
		xNear, yNear, -zNear,
		-xNear, yNear, -zNear,

		-xFar,-yFar,-zFar,
		xFar,-yFar,-zFar,
		xFar,yFar,-zFar,
		-xFar,yFar,-zFar
	};
	float vertexCount = sizeof(vertexes) / sizeof(float);
	//连接顺序，用于生成EBO
	unsigned int indices[] = {
		0,1,
		1,2,
		2,3,
		3,0,
		4,5,
		5,6,
		6,7,
		7,4,
		3,7,
		2,6,
		0,4,
		1,5
	};
	mVBO = CreateGPUBufferObject(GL_ARRAY_BUFFER, sizeof(vertexes), GL_STATIC_DRAW, vertexes);
	mEBO = CreateGPUBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), GL_STATIC_DRAW, indices);
}

void Frustum::Draw(const float* const& m, const float* const& v, const float* const& p)
{

	glUseProgram(mProgram);

	glUniformMatrix4fv(mMLocation, 1, GL_FALSE, m);
	glUniformMatrix4fv(mVLocation, 1, GL_FALSE, v);
	glUniformMatrix4fv(mPLocation, 1, GL_FALSE, p);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glEnableVertexAttribArray(mPosLocation);
	glVertexAttribPointer(mPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

