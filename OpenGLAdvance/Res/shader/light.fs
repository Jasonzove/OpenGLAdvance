#version 430

in vec3 V_normal;

void main()
{
    //ƽ�й����
    vec3 lightPosition = vec3(0.0,10.0,0.0);
    vec3 L = lightPosition;
    L = normalize(L);
    vec3 N = normalize(V_normal);

    //������
    vec4 ambientLightColor = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientColor = ambientLightColor*ambientMaterial;

    //diffuse
    vec4 diffuseLightColor = vec4(1.0,1.0,1.0,1.0);
    vec4 diffuseMaterial = vec4(0.4,0.4,0.4,1.0);
    //max(0.0,dot(L,N):��ֹ���ָ���,dot(L,N)Ϊǿ��
    vec4 diffuseColor = diffuseLightColor*diffuseMaterial*max(0.0,dot(L,N));

    gl_FragColor = ambientColor + diffuseColor;
}