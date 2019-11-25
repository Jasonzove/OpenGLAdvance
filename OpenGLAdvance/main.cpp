#include <stdio.h>
#include <windows.h>
#include "glew.h"
#include "Glm/glm.hpp"
#include "Glm/ext.hpp"
#include "tool.h"
#include "model.h"


/* 监听用户操作函数;LRESULT(函数返回值类型); CALLBACK(调用方式)
   hwnd(窗口句柄，用于标记用户操作了哪一个窗口); msg(消息ID，比如1表示用户拖拽了窗口);
   wParam(消息附带参数，比如用户拖拽窗口，具体拖到什么地方去了); lParam(消息附带参数)
*/
LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: //目前只关心退出窗口的消息
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* INT（函数返回值类型）; WINAPI（函数修饰符,强调调用方式）;
   操作系统启东时传入的参数：hInstance（当前应用程序的实例),hPrevInstance（上一次该应用程序启动的实例），
   lpCmdLine（命令行启东程序传入的参数）,nShowCmd（窗口显示相关参数）*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//创建窗口包含四步：注册，创建，显示，监听用户操作

	//注册窗口
	WNDCLASSEX windClass; //用此结构体告诉操作系统我们需要一个什么样的窗口
	windClass.cbClsExtra = NULL; //窗口类型的额外空间（在此我们不需要额外的空间）
	windClass.cbSize = sizeof(WNDCLASSEX); //windClass这个对象实际占用的内存
	windClass.cbWndExtra = 0; //窗口的额外内存
	windClass.hbrBackground = NULL; //使用OpenGL擦除背景色，而不是要用GDI所以为NULL
	windClass.hCursor = LoadCursor(NULL, IDC_ARROW); //设置光标（在此设置为箭头光标）
	windClass.hIcon = NULL; //应用程序在文件夹中显示的图标（在此不设置）
	windClass.hIconSm = NULL; //应用程序启动后在左上角显示的图标
	windClass.hInstance = hInstance; //操作系统传入的参数
	windClass.lpfnWndProc = GLWindowProc; //此函数监听用户操作，告诉我们谁操作了我们的窗口
	windClass.lpszClassName = L"GLWindow"; //窗口名称
	windClass.lpszMenuName = NULL; //菜单的名字（没有菜单，赋空）
	windClass.style = CS_VREDRAW | CS_HREDRAW; //窗口刷新时采用的重绘方式（在此采用水平或者垂直重绘的方式）
	ATOM atom = RegisterClassEx(&windClass);
	if (!atom)
	{
		MessageBox(NULL, L"Notice", L"Error", MB_OK);
		return 0;
	}

	//视口大小
	RECT rect;
	rect.left = 0;
	rect.right = 800;
	rect.top = 0;
	rect.bottom = 600;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, NULL); // 窗口风格和菜单句柄
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//创建窗口
	//参数依次对应为：窗口创建的风格，窗口名称（注册时的名称）, 窗口右上角标题栏，窗口的风格，窗帘起始位置，窗口的宽和高, 附窗口的句柄，菜单句柄， 窗口实例
	HWND hwnd = CreateWindowEx(NULL, L"GLWindow", L"Opengl Window", WS_OVERLAPPEDWINDOW, 100, 100, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	//创建opengl渲染环境
	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.cColorBits = 32; //一个像素占用32bit，RGBA分别占用8bit
	pfd.cDepthBits = 24; //深度缓冲区每个像素占24it，浮点数
	pfd.cStencilBits = 8; //蒙板缓冲区每个像素占8bit
	pfd.iPixelType = PFD_TYPE_RGBA; //设置像素类型
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //设置一下其他的flag
	int pixelFormat = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixelFormat, &pfd);
	HGLRC rc = wglCreateContext(dc); //渲染环境
	wglMakeCurrent(dc, rc); //使渲染环境生效

	//glewInit必须放在wglMakeCurrent之后
	glewInit();
	GLuint proram = CreateGPUProgram("Res/shader/texture.vs", "Res/shader/texture.fs"); //必须放在glewInit之后
	GLint posLoaction, texcoordLocation, normalLocation ,MLocation, VLocation, PLocation, normalMatrixLocation, mainTextureLocation;
	posLoaction = glGetAttribLocation(proram, "pos");
	texcoordLocation = glGetAttribLocation(proram, "texcoord");
	normalLocation = glGetAttribLocation(proram, "normal");
	MLocation = glGetUniformLocation(proram, "M");
	VLocation = glGetUniformLocation(proram, "V");
	PLocation = glGetUniformLocation(proram, "P");
	normalMatrixLocation = glGetUniformLocation(proram, "NormalMatrix");
	mainTextureLocation = glGetUniformLocation(proram, "U_MainTexture");

	unsigned int* indexes = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	
	//load obj model
	VertexData* vertexes = LoadObjModel("Res/model/Sphere.obj", &indexes, vertexCount, indexCount);

	//obj model -> vbo & ibo
	GLuint vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexCount, GL_STATIC_DRAW, vertexes);
	GLuint ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, GL_STATIC_DRAW, indexes);
	//texture
	GLuint mainTexture = CreateTextureFromFile("Res/image/niutou.bmp");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//数据准备
	float identity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//model mat:旋转，平移，缩放  //view mat:视口，摄像机漫游 //projection：3d->2d
	glm::mat4 modelMatrix = glm::translate(0.0f, 0.0f, -4.0f);
	glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
	//光照
	glm::mat4 normalMatrix = glm::inverseTranspose(modelMatrix);

	//用循环来保持窗口显示
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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(proram);
		glUniformMatrix4fv(MLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, identity);
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		//texture
		glBindTexture(GL_TEXTURE_2D, mainTexture);
		glUniform1i(mainTextureLocation, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLoaction);
		glVertexAttribPointer(posLoaction, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
		glEnableVertexAttribArray(texcoordLocation);
		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(normalLocation);
		glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(5 * sizeof(float)));
		
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		SwapBuffers(dc);
	}

	return 0;
}