#include <windows.h>
#include "glew.h"
#include "Glm/ext.hpp"
#include "Glm/glm.hpp"

#include "utils.h"
#include "objmodel.h"
#include "resource.h"
#include "shader_coder.h"

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

	HWND hwnd = CreateWindowEx(NULL, L"OpenGL", L"RenderWindow", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, hInstance, NULL);
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
	glewInit(); //glew初始化，必须放在wglMakeCurrent之后

	GLuint program = CreateGPUProgram(ShaderCoder::Get(IDR_SHADER_first_triangles_vs).c_str(),
		ShaderCoder::Get(IDR_SHADER_first_triangles_fs).c_str());
	GLuint posLocation, texcoordLocation, normalLocation, MLocation, VLocation, PLocation;
	posLocation = glGetAttribLocation(program, "pos");
	texcoordLocation = glGetAttribLocation(program, "texcoord");
	normalLocation = glGetAttribLocation(program, "normal");
	MLocation = glGetUniformLocation(program, "M");
	VLocation = glGetUniformLocation(program, "V");
	PLocation = glGetUniformLocation(program, "P");

	//obj model
	int* indexes = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	VertexData* vertexData = nullptr;
	vertexData = objModel.LoadObjModel("./res/model/niutou.obj", &indexes, vertexCount, indexCount);
	//vbo, ebo
	GLuint vbo = CreateGPUBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData)*vertexCount, GL_STATIC_DRAW, vertexData);
	GLuint ebo = CreateGPUBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indexCount, GL_STATIC_DRAW, indexes);

	glClearColor(41.0f / 255.0f, 71.0f / 255.0f, 121.0f / 255.0f, 1.0f);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	glm::mat4 projectionMat = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
	float idetity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float model[] = {
	0.5,0,0,0,
	0,0.5,0,0,
	0,0,0.5,0,
	0,-20,-100.0,1
	};

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
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);

		glUniformMatrix4fv(MLocation, 1, GL_FALSE, model);
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, idetity);
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projectionMat));

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLocation);
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)NULL);
		glEnableVertexAttribArray(texcoordLocation);
		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(normalLocation);
		glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(5 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		SwapBuffers(dc);
	}
	return 0;
}