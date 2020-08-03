#version 460

uniform sampler2D U_MainTexture;

in vec2 V_texcoord;

void main()
{
    float distance = length(V_texcoord*2.0 - 1.0)*3.0; //À­¿ª¾àÀë
    float alpha = exp(-distance*distance);
    if(alpha < 0.01)discard;

    gl_FragColor = vec4(vec3(0.1,0.5,0.7), alpha);
    //gl_FragColor = vec4(texture2D(U_MainTexture, V_texcoord).rgb, alpha);
}
