#version 460

void main()
{
    //�˴�Ҫȡ�ݲ��У�������
    gl_FragColor = vec4(vec3(pow(gl_FragCoord.z, 32.0)), 1.0);
}