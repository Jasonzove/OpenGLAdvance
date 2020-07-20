#version 430
in vec3 pos;
in vec4 color;

unifrom mat4 M;
unifrom mat4 V;
uniform mat4 P;

out vec4 v_color;

void main()
{
	gl_Position = P*V*M*vec4(pos, 1.0); //���һ��2D�ĵ�,�кܶ���̱��Կ������
	v_color = color;
}