#version 430 core

in vec2 v2f_TexCoords;
out vec4 FragColor_;

uniform sampler2D u_Image;
uniform bool u_IsHorizontal;
uniform float u_GaussianWeight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 TexelOffset = 1.0 / textureSize(u_Image, 0); //每个纹素的大小
    vec3 Result = texture(u_Image, v2f_TexCoords).rgb * u_GaussianWeight[0];
    if(u_IsHorizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
        Result += texture(u_Image, v2f_TexCoords + vec2(TexelOffset.x * i, 0.0)).rgb * u_GaussianWeight[i];
        Result += texture(u_Image, v2f_TexCoords - vec2(TexelOffset.x * i, 0.0)).rgb * u_GaussianWeight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            Result += texture(u_Image, v2f_TexCoords + vec2(0.0, TexelOffset.y * i)).rgb * u_GaussianWeight[i];
            Result += texture(u_Image, v2f_TexCoords - vec2(0.0, TexelOffset.y * i)).rgb * u_GaussianWeight[i];
        }
    }
    FragColor_ = vec4(Result, 1.0);
}