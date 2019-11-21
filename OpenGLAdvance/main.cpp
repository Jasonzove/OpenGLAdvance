#include <stdio.h>
#include <windows.h>
#include "glew.h"
#include "Glm/glm.hpp"
#include "Glm/ext.hpp"


struct Vertex
{
	float pos[3];
	float color[4];
};

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

char* LoadShaderContent(const char* path)
{
	FILE* pFile = fopen(path, "rb");
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int nlen = ftell(pFile);
		char* buffer = new char[nlen + 1];
		rewind(pFile);
		fread(buffer, nlen, 1, pFile);
		buffer[nlen] = '\0';
		fclose(pFile);
		return buffer;
	}
	fclose(pFile);
	return nullptr;
}

GLuint CreateGPUProgram(const char* vsShaderPath, const char* fsShaderPath)
{
	GLuint vsShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* vsCode = LoadShaderContent(vsShaderPath);
	const char* fsCode = LoadShaderContent(fsShaderPath);
	//传入GPU
	glShaderSource(vsShader, 1, &vsCode, nullptr);
	glShaderSource(fsShader, 1, &fsCode, nullptr);
	//编译
	glCompileShader(vsShader);
	glCompileShader(fsShader);
	//创建program并attach
	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	//Link
	glLinkProgram(program);
	//detach and delete
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);

	return program;
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
	GLuint proram = CreateGPUProgram("sample.vs", "sample.fs"); //必须放在glewInit之后
	GLint posLoaction, colorLocation, MLocation, VLocation, PLocation;
	posLoaction = glGetAttribLocation(proram, "pos");
	colorLocation = glGetAttribLocation(proram, "color");
	MLocation = glGetUniformLocation(proram, "M");
	VLocation = glGetUniformLocation(proram, "V");
	PLocation = glGetUniformLocation(proram, "P");

	Vertex vertexs[3];
	vertexs[0].pos[0] = 0.0f;
	vertexs[0].pos[1] = 0.0f;
	vertexs[0].pos[2] = -5.0f;
	vertexs[0].color[0] = 1.0f;
	vertexs[0].color[1] = 1.0f;
	vertexs[0].color[2] = 1.0f;
	vertexs[0].color[3] = 1.0f;

	vertexs[1].pos[0] = 2.0f;
	vertexs[1].pos[1] = 0.0f;
	vertexs[1].pos[2] = -5.0f;
	vertexs[1].color[0] = 1.0f;
	vertexs[1].color[1] = 1.0f;
	vertexs[1].color[2] = 1.0f;
	vertexs[1].color[3] = 1.0f;

	vertexs[2].pos[0] = 0.0f;
	vertexs[2].pos[1] = 2.0f;
	vertexs[2].pos[2] = -5.0f;
	vertexs[2].color[0] = 1.0f;
	vertexs[2].color[1] = 1.0f;
	vertexs[2].color[2] = 1.0f;
	vertexs[2].color[3] = 1.0f;

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3, vertexs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indexes[] = { 0,1,2 };
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(unsigned int), indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//数据准备
	float identity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	glm::mat4 projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);

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

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(proram);
		glUniformMatrix4fv(MLocation, 1, GL_FALSE, identity);
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, identity);
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projection));

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(posLoaction);
		glVertexAttribPointer(posLoaction, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(colorLocation);
		glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		SwapBuffers(dc);
	}

	return 0;
}