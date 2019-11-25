varying vec3 V_Normal;
varying vec4 V_WorldPos;
varying vec2 V_Texcoord;

uniform sampler2D U_MainTexture;


void main()
{
    vec3 lightPos=vec3(10.0, 10.0, 0.0); //ƽ�й�
    vec3 L = lightPos; //�㵽��Դ����������Ϊ��ƽ�й⣬�������е������һ��
    L = normalize(L);
    vec3 n = normalize(V_Normal);

    //ambient
    vec4 AmbientLightColor = vec4(0.2,0.2,0.2,1.0);
    vec4 AmbientMaterial = vec4(0.2,0.2,0.2, 1.0); //ֻ����0.2 -- -- 
    vec4 ambientColor = AmbientLightColor*AmbientMaterial;

    //diffuse
    vec4 DiffuseLightColor = vec4(1.0,1.0,1.0,1.0);
    vec4 DiffuseMaterial = vec4(0.2, 0.2, 0.2, 1.0);
    vec4 diffuseColor = DiffuseLightColor*DiffuseMaterial*max(0.0, dot(L,n)); //max(0.0, dot(L,n):Ӱ�����ǿ��

    //specular
    vec4 SpecularLightColor = vec4(1.0,1.0,1.0,1.0);
    vec4 SpecularMaterial = vec4(0.8,0.8,0.8,1.0);
    vec3 reflectDir = normalize(reflect(-L,n)); //������߷���
    vec3 viewDir = normalize(vec3(0.0) - V_WorldPos.xyz); //�㵽�۾��ķ���
    vec4 specularColor = SpecularLightColor*SpecularMaterial*pow(max(0.0, dot(viewDir, reflectDir)), 64);

    gl_FragColor = texture2D(U_MainTexture, V_Texcoord);//ambientColor + diffuseColor + specularColor;
}