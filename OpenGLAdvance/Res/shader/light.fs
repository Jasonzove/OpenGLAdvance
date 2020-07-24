#version 430

in vec3 V_normal;

void main()
{
    //平行光参数
    vec3 lightPosition = vec3(0.0,10.0,0.0);
    vec3 L = lightPosition;
    L = normalize(L);
    vec3 N = normalize(V_normal);

    //环境光
    vec4 ambientLightColor = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientColor = ambientLightColor*ambientMaterial;

    //diffuse
    vec4 diffuseLightColor = vec4(1.0,1.0,1.0,1.0);
    vec4 diffuseMaterial = vec4(0.4,0.4,0.4,1.0);
    //max(0.0,dot(L,N):防止出现负数,dot(L,N)为强度
    vec4 diffuseColor = diffuseLightColor*diffuseMaterial*max(0.0,dot(L,N));

    gl_FragColor = ambientColor + diffuseColor;
}