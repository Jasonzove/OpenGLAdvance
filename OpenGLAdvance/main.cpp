#include <windows.h>
#include "glew.h"
#include "wglew.h"
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

HGLRC CreateNBRC(HDC dc)
{
	HGLRC rc = nullptr;
	GLint attribs[] = { //������Ⱦ�豸����
		WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE, //�������ز���
		WGL_SAMPLES_ARB, 16,//16�ز���
		NULL, NULL
	};

	int pixelFormat[256];
	memset(pixelFormat, 0, 256 * sizeof(int));
	UINT formatNum = 0;
	wglChoosePixelFormatARB(dc, attribs, NULL, 256, pixelFormat, &formatNum);
	printf("support attribte format number is:%u\n", formatNum);
	if (formatNum > 0)
	{
		//���Կ�������ѡ�����ظ�ʽ
		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(dc, pixelFormat[0], sizeof(pfd), &pfd);
		SetPixelFormat(dc, pixelFormat[0], &pfd);

		int contextAttributs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, //һ��Ҫѡ���ּ���ģʽ
			0
		};

		rc = wglCreateContextAttribsARB(dc, nullptr, contextAttributs);
	}

	return rc;
}

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
	//����Ҫ�����Ĵ��ڵĴ�С
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

	glewInit(); //glew��ʼ�����������wglMakeCurrent֮��
	if (wglChoosePixelFormatARB) // �ж�Ӳ���Ƿ�֧�ֶ��ز��������
	{
		printf("wglChoosePixelFormatARB != nullptr\n");
		//������һ��window������һ��RC�ڴ����µ�RC��ֻ�����ٴ������´���
		//destory window
		wglMakeCurrent(dc, nullptr);
		wglDeleteContext(rc);
		rc = nullptr;
		ReleaseDC(hwnd, dc);
		dc = nullptr;
		DestroyWindow(hwnd);
		hwnd = CreateWindowEx(NULL, L"OpenGL", L"RenderWindow", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
		dc = GetDC(hwnd);
		rc = CreateNBRC(dc);
		wglMakeCurrent(dc, rc);
	}

	int width = 0;
	int height = 0;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	GLuint program = CreateGPUProgram(ShaderCoder::Get(IDR_SHADER_simpleSSBO_vs).c_str(),
		ShaderCoder::Get(IDR_SHADER_mix_light_fs).c_str());
	GLuint posLocation, texcoordLocation, normalLocation, MLocation, VLocation, PLocation, normalmatLocation;
	GLuint textureSamplerLocation, offsetLocation, surfaceColorLocation;
	posLocation = glGetAttribLocation(program, "pos");
	texcoordLocation = glGetAttribLocation(program, "texcoord");
	normalLocation = glGetAttribLocation(program, "normal");
	MLocation = glGetUniformLocation(program, "M");
	VLocation = glGetUniformLocation(program, "V");
	PLocation = glGetUniformLocation(program, "P");
	normalmatLocation = glGetUniformLocation(program, "NormalMat");
	textureSamplerLocation = glGetUniformLocation(program, "U_MainTexture");
	offsetLocation = glGetAttribLocation(program, "offset");
	//subroutine
	surfaceColorLocation = glGetSubroutineUniformLocation(program, GL_FRAGMENT_SHADER, "U_SurfaceColor");
	GLuint ambientLightIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "Ambient");
	GLuint diffuseLightIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "Diffuse");
	GLuint specularLightIndex = glGetSubroutineIndex(program, GL_FRAGMENT_SHADER, "Specular");

	//obj model
	int* indexes = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	VertexData* vertexData = nullptr;
	vertexData = objModel.LoadObjModel("./res/model/niutou.obj", &indexes, vertexCount, indexCount);
	//vbo, ebo
	GLuint ebo = CreateGPUBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indexCount, GL_STATIC_DRAW, indexes);
	GLuint vao = CreatVAO([&](void)->void { //VAOֻ�Ƕ�VBO����״̬�ķ�װ�����ݻ��Ǵ���VBO����
		GLuint vbo = CreateGPUBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData)*vertexCount, GL_STATIC_DRAW, vertexData);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLocation);
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)NULL);
		glEnableVertexAttribArray(texcoordLocation);
		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(normalLocation);
		glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(5 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	});
	GLuint ssbo = CreateGPUBufferObject(GL_SHADER_STORAGE_BUFFER, sizeof(VertexData)*vertexCount, GL_STATIC_DRAW, vertexData);
	//instancing 
	//float posOffest[] = {-1.0f, 0.0f, 0.0f,
	//0.0f,0.0f,0.0f,
	//1.0f,0.0f,0.0f};
	//GLuint offesetVBO = CreateGPUBufferObject(GL_ARRAY_BUFFER, sizeof(posOffest), GL_STATIC_DRAW, posOffest);

	//texture
	GLuint textureId = CreateTexture("./res/image/niutou.bmp");
	//GLuint textureId = ReverseColor("./res/image/niutou.bmp");
	//fbo
	GLuint colorBuffer, colorBuffer2, depthBuffer;
	//���ִ�����ʽ�������Ч�ģ�����ͨ�õģ���Ч�� �Լ��о�
	GLuint fbo = CreateFrameBufferObject(colorBuffer, depthBuffer, width, height, &colorBuffer2);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//glClearColor(0.1f, 0.4f, 0.7f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	////��ɫ
	//GLuint computeProgram = CreateComputeProgram(ShaderCoder::Get(IDR_SHADER_simple_compute).c_str());
	//GLuint computeTexture = CreateComputeTexture(width, height);
	//Timer t;
	//t.Start();
	//glUseProgram(computeProgram);
	////����
	//glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	////���
	//glBindImageTexture(1, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	//glDispatchCompute(width / 16, height / 16, 1);
	////sync cpu with GPU:ͬ��
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//glUseProgram(0);
	//printf("compute shader time:%f ms\n", t.GetPassedTimeInMs());

	//glClearColor(41.0f / 255.0f, 71.0f / 255.0f, 121.0f / 255.0f, 1.0f);
	glClearColor(0.1f, 0.4f, 0.7f, 1.0f);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_POINT_SPRITE); //�����㾫����ͼ
	//glEnable(GL_PROGRAM_POINT_SIZE); //�����㾫���С

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0.0f, 0.0f, (float)width, (float)height);

	glm::mat4 projectionMat = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	//2d��������ԭͼ��1*1���ص�
	//glm::mat4 uiMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f); //����Ļһ���󣬱�֤������
	//glm::mat4 uiMat = glm::ortho(-4.0f/8.0f, 4.0f / 8.0f, -3.0f / 8.0f, 3.0f / 8.0f);
	float idetity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	//glm::mat4 modelMat = glm::scale(100.0f, 100.0f, 1.0f); //һ�����أ��Ŵ�100�����ܿ���
	glm::mat4 modelMat = glm::translate(0.0f, -0.5f, -2.0f)*glm::rotate(90.0f, 0.0f, -1.0f, 0.0f)*glm::scale(0.01f, 0.01f, 0.01f);
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

		//glBindVertexArray(vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &specularLightIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		//glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, 3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

		//glBindVertexArray(0);
		glUseProgram(0);
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
		//angle += 0.5f;
		//if (angle > 360.0f)
		//{
		//	angle = 0.0f;
		//}
		//modelMat = glm::translate(0.0f, 0.0f, -4.0f) * glm::rotate(angle, 0.0f, 1.0f, 0.0f);
		//glm::mat4 normalMat = glm::inverseTranspose(modelMat); //model������Ҫ����normalmat,normalmat�����þ��ǽ����ߴӾֲ�����ϵת����������ϵ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render();
		SwapBuffers(dc);
	}
	return 0;
}