#version 430

in vec3 pos;
in vec2 texcoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 V_Texcoord;

void main()
{
	V_Texcoord = texcoord;
    gl_Position = vec4(pos.x*2, pos.y*2, pos.z, 1.0);//Éæ¼°NDCµÄ¸ÅÄî
}