#version 430
in vec3 pos;
in vec4 color;

unifrom mat4 M;
unifrom mat4 V;
uniform mat4 P;

out vec4 v_color;

void main()
{
	gl_Position = P*V*M*vec4(pos, 1.0); //变成一个2D的点,有很多过程被显卡处理掉
	v_color = color;
}