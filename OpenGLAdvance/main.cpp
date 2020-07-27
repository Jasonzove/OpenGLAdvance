#include <windows.h>
#include "glew.h"
#include "Glm/ext.hpp"
#include "Glm/glm.hpp"

#include "utils.h"
#include "objmodel.h"
#include "resource.h"
#include "shader_coder.h"
#include "timer.h"
#include "frustum.h"

//#pragma  comment(lib, "opengl32.lib")
#pragma  comment(lib, "glew32.lib")

ObjModel objModel;

LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hIconSm = NULL;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = GLWindowProc;
	wndClass.lpszClassName = L"OpenGL";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndClass);
	//设置要创建的窗口的大小
	RECT rect;
	rect.left = 0.0f;
	rect.right = 800.0f;
	rect.bottom = 600.0f;
	rect.top = 0.0f;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowEx(NULL, L"OpenGL", L"RenderWindow", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_TYPE_RGBA | PFD_DOUBLEBUFFER;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixelFormatID = ChoosePixelFormat(dc, &pfd);

	SetPixelFormat(dc, pixelFormatID, &pfd);

	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);

	int width = 0;
	int height = 0;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	glewInit(); //glew初始化，必须放在wglMakeCurrent之后

	GLuint program = CreateGPUProgram(ShaderCoder::Get(IDR_SHADER_point_sprite_vs).c_str(),
		ShaderCoder::Get(IDR_SHADER_point_sprite_fs).c_str());
	GLuint posLocation, texcoordLocation, normalLocation, MLocation, VLocation, PLocation, normalmatLocation;
	GLuint textureSamplerLocation;
	posLocation = glGetAttribLocation(program, "pos");
	texcoordLocation = glGetAttribLocation(program, "texcoord");
	normalLocation = glGetAttribLocation(program, "normal");
	MLocation = glGetUniformLocation(program, "M");
	VLocation = glGetUniformLocation(program, "V");
	PLocation = glGetUniformLocation(program, "P");
	normalmatLocation = glGetUniformLocation(program, "NormalMat");
	textureSamplerLocation = glGetUniformLocation(program, "U_MainTexture");

	//obj model
	int* indexes = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	VertexData* vertexData = nullptr;
	vertexData = objModel.LoadObjModel("./res/model/Quad.obj", &indexes, vertexCount, indexCount);
	//vbo, ebo
	vertexData[0].position[0] = 0.0f;
	vertexData[0].position[1] = 0.0f; //点精灵的朝向永远朝一个方向，旋转不起作用
	GLuint vbo = CreateGPUBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData)*vertexCount, GL_STATIC_DRAW, vertexData);
	GLuint ebo = CreateGPUBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indexCount, GL_STATIC_DRAW, indexes);
	//texture
	GLuint textureId = CreateTexture("./res/image/camera.dds");

	//glClearColor(41.0f / 255.0f, 71.0f / 255.0f, 121.0f / 255.0f, 1.0f);
	glClearColor(0.1f, 0.4f, 0.7f, 1.0f);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_POINT_SPRITE); //开启点精灵贴图
	glEnable(GL_PROGRAM_POINT_SIZE); //开启点精灵大小

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0.0f, 0.0f, (float)width, (float)height);

	glm::mat4 projectionMat = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	Frustum frustum;
	frustum.InitProgram();
	frustum.InitPerspective(45.0f, (float)width / (float)height, 0.1f, 4.0f);
	//2d，调整，原图是1*1像素的
	//glm::mat4 uiMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f); //和屏幕一样大，保证不变形
	//glm::mat4 uiMat = glm::ortho(-4.0f/8.0f, 4.0f / 8.0f, -3.0f / 8.0f, 3.0f / 8.0f);
	float idetity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	//glm::mat4 modelMat = glm::scale(100.0f, 100.0f, 1.0f); //一个像素，放大100倍才能看见
	glm::mat4 modelMat = glm::translate(-2.0f, 0.0f, -4.0f)*glm::rotate(-20.0f, 0.0f,1.0f,0.0f);
	glm::mat4 normalMat = glm::inverseTranspose(modelMat);
	static float angle;

	auto render = [&](void)->void {
		glUseProgram(program);

		glUniformMatrix4fv(MLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, idetity);
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniformMatrix4fv(normalmatLocation, 1, GL_FALSE, glm::value_ptr(normalMat));

		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(textureSamplerLocation, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLocation);
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)NULL);
		glEnableVertexAttribArray(texcoordLocation);
		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(normalLocation);
		glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(5 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
	};
	//GL_CHECK(glEnable(GL_LINE));
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//angle += 0.5f;
		//if (angle > 360.0f)
		//{
		//	angle = 0.0f;
		//}
		//modelMat = glm::translate(0.0f, 0.0f, -4.0f) * glm::rotate(angle, 0.0f, 1.0f, 0.0f);
		//glm::mat4 normalMat = glm::inverseTranspose(modelMat); //model更新需要更新normalmat,normalmat的作用就是讲法线从局部坐标系转到世界坐标系
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		frustum.Draw(glm::value_ptr(modelMat), idetity, glm::value_ptr(projectionMat));

		render();
		SwapBuffers(dc);
	}
	return 0;
}