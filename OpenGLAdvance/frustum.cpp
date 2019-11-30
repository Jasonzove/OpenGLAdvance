#include "frustum.h"
#include "tool.h"
#include "Glm/ext.hpp"
#include "Glm/glm.hpp"

void Frustum::InitProgram()
{
	mProgram = CreateGPUProgram("Res/shader/white_color.vs", "Res/shader/white_color.fs");
	mPosLocation = glGetAttribLocation(mProgram, "pos");
	mMLocation = glGetUniformLocation(mProgram, "M");
	mVLocation = glGetUniformLocation(mProgram, "V");
	mPLocation = glGetUniformLocation(mProgram, "P");
}

void Frustum::InitPerspective(const float& fov, const float& aspect, const float& zNear, const float& zFar)
{
	float halfFov = fov / 2;
	float radiusHalfFov = 3.14*halfFov / 180.0f; // »¡¶È
	float tanHalfFov = sinf(radiusHalfFov) / cosf(radiusHalfFov);
	float yNear = zNear * tanHalfFov;
	float xNear = yNear * aspect;
	float yFar = zFar * tanHalfFov;
	float xFar = zFar * tanHalfFov;
	
	//vbo
	float vertexes[] = {
		-xNear, -yNear, -zNear,
		xNear, -yNear, -zNear,
		xNear, yNear, -zNear,
		-xNear, yNear, -zNear,

		-xFar, -yFar, -zFar,
		xFar, -yFar, -zFar,
		xFar, yFar, -zFar,
		-xFar, yFar, -zFar,
	};
	mVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(vertexes), GL_STATIC_DRAW, vertexes);

	//ibo
	int indices[] = {
		0,1,1,2,2,3,3,0,
		4,5,5,6,6,7,7,4,
		0,4,1,5,2,6,3,7
	};
	mIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), GL_STATIC_DRAW, indices);
}

void Frustum::InitOrtho(const float& left, const float& right, const float bottom, const float& top, const float& zNear, const float& zFar)
{
	//vbo
	float vertexes[] = {
		-left, -bottom, -zNear,
		left, -bottom, -zNear,
		right, top, -zNear,
		-left, top, -zNear,

		-left, -bottom, -zFar,
		left, -bottom, -zFar,
		right, top, -zFar,
		-left, top, -zFar,
	};
	mVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(vertexes), GL_STATIC_DRAW, vertexes);

	//ibo
	int indices[] = {
		0,1,1,2,2,3,3,0,
		4,5,5,6,6,7,7,4,
		0,4,1,5,2,6,3,7
	};
	mIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), GL_STATIC_DRAW, indices);
}

void Frustum::Draw(const float* const& M, const float* const& V, const float* const& P)
{
	glUseProgram(mProgram);
	glUniformMatrix4fv(mMLocation, 1, GL_FALSE, M);
	glUniformMatrix4fv(mVLocation, 1, GL_FALSE, V);
	glUniformMatrix4fv(mPLocation, 1, GL_FALSE, P);

	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glEnableVertexAttribArray(mPosLocation);
	glVertexAttribPointer(mPosLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIbo);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}