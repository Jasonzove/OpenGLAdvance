#version 460

void main()
{
    //此处要取幂才行？？？？
    gl_FragColor = vec4(vec3(pow(gl_FragCoord.z, 32.0)), 1.0);
}