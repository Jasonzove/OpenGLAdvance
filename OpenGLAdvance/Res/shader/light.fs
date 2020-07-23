#version 430

void main()
{
    //»·¾³¹â
    vec4 ambientColor = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambient = ambientColor*ambientMaterial;
    gl_FragColor = ambient;
}