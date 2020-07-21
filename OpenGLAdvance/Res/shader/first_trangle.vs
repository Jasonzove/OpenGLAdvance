#version 430
in vec3 pos;
in vec4 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 V_Color;

void main()
{
    V_Color = color;
    gl_Position = P*V*M*vec4(pos, 1.0);
}