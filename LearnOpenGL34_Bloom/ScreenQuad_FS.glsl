#version 430 core

in vec2 v2f_TexCoords;
out vec4 Color_;

uniform sampler2D u_HDRTexture;
uniform sampler2D u_BluredTexture;

void main()
{
	vec3 HDRColor = texture(u_HDRTexture, v2f_TexCoords).rgb;
	vec3 BloomColor = texture(u_BluredTexture, v2f_TexCoords).rgb;
	HDRColor += BloomColor;
	//�ع�ɫ��ӳ�䣬�ع��1.0
	float Exposure = 1.0;
	HDRColor = vec3(1.0) - exp(-HDRColor * Exposure);
	//GammaУ��
	HDRColor = pow(HDRColor, vec3(1.0 / 2.2));
	Color_ = vec4(HDRColor, 1.0);
}