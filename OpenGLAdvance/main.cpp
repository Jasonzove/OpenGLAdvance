#include <stdio.h>
#include <windows.h>
#include "glew.h"
#include "Glm/glm.hpp"
#include "Glm/ext.hpp"
#include "tool.h"
#include "model.h"
#include "timer.h"
#include "frustum.h"


/* �����û���������;LRESULT(��������ֵ����); CALLBACK(���÷�ʽ)
   hwnd(���ھ�������ڱ���û���������һ������); msg(��ϢID������1��ʾ�û���ק�˴���);
   wParam(��Ϣ���������������û���ק���ڣ������ϵ�ʲô�ط�ȥ��); lParam(��Ϣ��������)
*/
LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: //Ŀǰֻ�����˳����ڵ���Ϣ
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* INT����������ֵ���ͣ�; WINAPI���������η�,ǿ�����÷�ʽ��;
   ����ϵͳ����ʱ����Ĳ�����hInstance����ǰӦ�ó����ʵ��),hPrevInstance����һ�θ�Ӧ�ó���������ʵ������
   lpCmdLine������������������Ĳ�����,nShowCmd��������ʾ��ز�����*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//�������ڰ����Ĳ���ע�ᣬ��������ʾ�������û�����

	//ע�ᴰ��
	WNDCLASSEX windClass; //�ô˽ṹ����߲���ϵͳ������Ҫһ��ʲô���Ĵ���
	windClass.cbClsExtra = NULL; //�������͵Ķ���ռ䣨�ڴ����ǲ���Ҫ����Ŀռ䣩
	windClass.cbSize = sizeof(WNDCLASSEX); //windClass�������ʵ��ռ�õ��ڴ�
	windClass.cbWndExtra = 0; //���ڵĶ����ڴ�
	windClass.hbrBackground = NULL; //ʹ��OpenGL��������ɫ��������Ҫ��GDI����ΪNULL
	windClass.hCursor = LoadCursor(NULL, IDC_ARROW); //���ù�꣨�ڴ�����Ϊ��ͷ��꣩
	windClass.hIcon = NULL; //Ӧ�ó������ļ�������ʾ��ͼ�꣨�ڴ˲����ã�
	windClass.hIconSm = NULL; //Ӧ�ó��������������Ͻ���ʾ��ͼ��
	windClass.hInstance = hInstance; //����ϵͳ����Ĳ���
	windClass.lpfnWndProc = GLWindowProc; //�˺��������û���������������˭���������ǵĴ���
	windClass.lpszClassName = L"GLWindow"; //��������
	windClass.lpszMenuName = NULL; //�˵������֣�û�в˵������գ�
	windClass.style = CS_VREDRAW | CS_HREDRAW; //����ˢ��ʱ���õ��ػ淽ʽ���ڴ˲���ˮƽ���ߴ�ֱ�ػ�ķ�ʽ��
	ATOM atom = RegisterClassEx(&windClass);
	if (!atom)
	{
		MessageBox(NULL, L"Notice", L"Error", MB_OK);
		return 0;
	}

	//�ӿڴ�С
	RECT rect;
	rect.left = 0;
	rect.right = 800;
	rect.top = 0;
	rect.bottom = 600;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, NULL); // ���ڷ��Ͳ˵����
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//��������
	//�������ζ�ӦΪ�����ڴ����ķ�񣬴������ƣ�ע��ʱ�����ƣ�, �������ϽǱ����������ڵķ�񣬴�����ʼλ�ã����ڵĿ�͸�, �����ڵľ�����˵������ ����ʵ��
	HWND hwnd = CreateWindowEx(NULL, L"GLWindow", L"Opengl Window", WS_OVERLAPPEDWINDOW, 100, 100, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	//����opengl��Ⱦ����
	HDC dc = GetDC(hwnd);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.cColorBits = 32; //һ������ռ��32bit��RGBA�ֱ�ռ��8bit
	pfd.cDepthBits = 24; //��Ȼ�����ÿ������ռ24it��������
	pfd.cStencilBits = 8; //�ɰ建����ÿ������ռ8bit
	pfd.iPixelType = PFD_TYPE_RGBA; //������������
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; //����һ��������flag
	int pixelFormat = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixelFormat, &pfd);
	HGLRC rc = wglCreateContext(dc); //��Ⱦ����
	wglMakeCurrent(dc, rc); //ʹ��Ⱦ������Ч


	//glewInit�������wglMakeCurrent֮��
	glewInit();
	GLuint proram = CreateGPUProgram("Res/shader/mix_light.vs", "Res/shader/mix_light.fs"); //�������glewInit֮��
	GLint posLoaction, texcoordLocation, normalLocation ,MLocation, VLocation, PLocation, normalMatrixLocation, mainTextureLocation, offesetLocation;
	GLint surfaceColorLocation;
	posLoaction = glGetAttribLocation(proram, "pos");
	texcoordLocation = glGetAttribLocation(proram, "texcoord");
	normalLocation = glGetAttribLocation(proram, "normal");
	MLocation = glGetUniformLocation(proram, "M");
	VLocation = glGetUniformLocation(proram, "V");
	PLocation = glGetUniformLocation(proram, "P");
	normalMatrixLocation = glGetUniformLocation(proram, "NormalMatrix");
	mainTextureLocation = glGetUniformLocation(proram, "U_MainTexture");
	offesetLocation = glGetAttribLocation(proram, "offeset");
	surfaceColorLocation = glGetSubroutineUniformLocation(proram, GL_FRAGMENT_SHADER, "U_SurfaceColor");
	GLuint ambientColorIndex = glGetSubroutineIndex(proram, GL_FRAGMENT_SHADER, "Ambient");
	GLuint diffuseColorIndex = glGetSubroutineIndex(proram, GL_FRAGMENT_SHADER, "Diffuse");
	GLuint specularColorIndex = glGetSubroutineIndex(proram, GL_FRAGMENT_SHADER, "Specular");

	unsigned int* indexes = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	
	//load obj model
	VertexData* vertexes = LoadObjModel("Res/model/niutou.obj", &indexes, vertexCount, indexCount);

	//�㾫���Ƶ�����
	//vertexes[0].position[0] = 0.0f;
	//vertexes[0].position[1] = 0.0f;

	//fov
	float fov = 45.0f;
	//cpu��UI������Ļ,���quad.obj
	//float z = 4.0f; //glm::mat4 modelMatrix = glm::translate(0.0f, 0.0f, -4.0f);
	//float halfFov = fov / 2.0f;
	//float radianHalfFov = (halfFov / 180.0)*3.14;
	//float tanHalfFov = sin(radianHalfFov) / cos(radianHalfFov);
	//float y = z * tanHalfFov;
	//float x = y * ((float)windowWidth / (float)windowHeight);
	//vertexes[0].position[0] = -x; //���½�
	//vertexes[0].position[1] = -y;
	//vertexes[1].position[0] = x; //���½�
	//vertexes[1].position[1] = -y;
	//vertexes[2].position[0] = -x; //���Ͻ�
	//vertexes[2].position[1] = y;
	//vertexes[3].position[0] = x; //���Ͻ�
	//vertexes[3].position[1] = y;

	//timer
	//Timer timer;
	//timer.Start();

	//obj model -> vbo & ibo
	GLuint vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * vertexCount, GL_STATIC_DRAW, vertexes);
	GLuint ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, GL_STATIC_DRAW, indexes);
	//instance
	float offeset[] = {
		-1.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f
	};
	GLuint offesetVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(offeset), GL_STATIC_DRAW, offeset);

	//texture
	GLuint mainTexture = CreateTextureFromFile("Res/image/niutou.bmp");

	//printf("time: %f ms\n", timer.GetPassedTime());

	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//�����㾫��
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//GL_CALL(glBegin(GL_LINEAR));

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	glViewport(0, 0, windowWidth, windowHeight);
	//����׼��
	float identity[] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	//model mat:��ת��ƽ�ƣ�����  //view mat:�ӿڣ���������� //projection��3d->2d
	glm::mat4 modelMatrix = glm::translate(0.0f, 0.0f, -4.0f)*glm::rotate(90.0f, 0.0f, -1.0f, 0.0f)*glm::scale(0.01f,0.01f,0.01f);
	glm::mat4 projection = glm::perspective(fov, (float)windowWidth/(float)windowHeight, 0.1f, 1000.0f);
	glm::mat4 projection2D = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f);
	//����
	glm::mat4 normalMatrix = glm::inverseTranspose(modelMatrix);
	//��ת
	//float angle = 0.0f;

	//��׵��
	Frustum frustum;
	frustum.InitProgram();
	//frustum.InitPerspective(fov, (float)windowWidth / (float)windowHeight, 0.1f, 4.0f);
	frustum.InitOrtho(0.5f, 0.5f, 0.5f, 0.5f, 0.1f, 4.0f);

	//��ѭ�������ִ�����ʾ
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
		//��ת
		//angle += 0.1;
		//if (angle > 360.f)
		//{
		//	angle = 0.0f;
		//}
		//modelMatrix = glm::translate(0.0f, 0.0f, -4.0f)*glm::rotate(angle, 0.0f, 1.0f, 0.0f);
		//normalMatrix = glm::inverseTranspose(modelMatrix);

		//����
		//glEnable(GL_SCISSOR_TEST);
		//glScissor(0, 0, windowWidth, 100);

		glUseProgram(proram);
		glUniformMatrix4fv(MLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(VLocation, 1, GL_FALSE, identity);
		glUniformMatrix4fv(PLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		//shader�Ӻ���
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ambientColorIndex);

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

		//instance
		//glBindBuffer(GL_ARRAY_BUFFER, offesetVbo);
		//glEnableVertexAttribArray(offesetLocation);
		//glVertexAttribPointer(offesetLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glVertexAttribDivisor(offesetLocation, 1); //ÿ����һ����ƫ��

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES , indexCount, GL_UNSIGNED_INT, 0);
		//glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, 3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		//glDisable(GL_SCISSOR_TEST);

		//��׵��
		//frustum.Draw(glm::value_ptr(modelMatrix), identity, glm::value_ptr(projection));

		SwapBuffers(dc);
	}

	return 0;
}