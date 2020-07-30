#version 460
subroutine vec4 SurfaceColor();

uniform sampler2D U_MainTexture;
subroutine uniform SurfaceColor U_SurfaceColor;

in vec3 V_normal;
in vec4 V_pos;
in vec2 V_texcoord;

subroutine (SurfaceColor) vec4 Ambient()
{
    //环境光
    vec4 ambientLightColor = vec4(0.4,0.4,0.4,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientColor = ambientLightColor*ambientMaterial;
    return ambientColor*texture2D(U_MainTexture, V_texcoord);
}

subroutine (SurfaceColor) vec4 Diffuse()
{
     //平行光参数
    vec3 lightPosition = vec3(10.0,10.0,0.0);
    vec3 L = lightPosition;
    L = normalize(L);
    vec3 N = normalize(V_normal);

    //环境光
    vec4 ambientLightColor = vec4(0.4,0.4,0.4,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientColor = ambientLightColor*ambientMaterial;

    //diffuse
    vec4 diffuseLightColor = vec4(0.8,0.8,0.8,1.0);
    vec4 diffuseMaterial = vec4(0.4,0.4,0.4,1.0);
    //max(0.0,dot(L,N):防止出现负数,dot(L,N)为强度
    vec4 diffuseColor = diffuseLightColor*diffuseMaterial*max(0.0,dot(L,N));
    return ambientColor + texture2D(U_MainTexture, V_texcoord)*diffuseColor;
}

subroutine (SurfaceColor) vec4 Specular()
{
     //平行光参数
    vec3 lightPosition = vec3(10.0,10.0,0.0);
    vec3 L = lightPosition;
    L = normalize(L);
    vec3 N = normalize(V_normal);

    //环境光
    vec4 ambientLightColor = vec4(0.4,0.4,0.4,1.0);
    vec4 ambientMaterial = vec4(0.2,0.2,0.2,1.0);
    vec4 ambientColor = ambientLightColor*ambientMaterial;

    //diffuse
    vec4 diffuseLightColor = vec4(0.8,0.8,0.8,1.0);
    vec4 diffuseMaterial = vec4(0.4,0.4,0.4,1.0);
    //max(0.0,dot(L,N):防止出现负数,dot(L,N)为强度
    vec4 diffuseColor = diffuseLightColor*diffuseMaterial*max(0.0,dot(L,N));

    //specular
    vec4 specularLightColor = vec4(1.0,1.0,1.0,1.0);
    vec4 specularMaterial = vec4(0.8,0.8,0.8,1.0);
    vec3 reflectDir = normalize(reflect(-L, N));
    //inverse view direction
    vec3 viewDir = normalize(vec3(0.0) - V_pos.xyz);
    vec4 specularColor = specularLightColor*specularMaterial*pow(max(0.0, dot(reflectDir, viewDir)),128);
    
    if(diffuseColor.r == 0) //背光面不出现光斑
    {
        gl_FragColor = vec4(0.0);  
    }

    return ambientColor + texture2D(U_MainTexture, V_texcoord) * (diffuseColor) + specularColor;
}

void main()
{
    //gl_FragColor = U_SurfaceColor();
    gl_FragColor = texture2D(U_MainTexture, V_texcoord);
}