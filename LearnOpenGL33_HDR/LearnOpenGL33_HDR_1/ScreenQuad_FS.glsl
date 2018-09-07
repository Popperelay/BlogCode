#version 430 core

in vec2 v2f_TexCoords;
out vec4 Color_;

uniform sampler2D u_HDRTexture;
uniform int u_ToneMapppingType;
uniform float u_;

void main()
{
	vec3 HDRColor = texture(u_HDRTexture, v2f_TexCoords).rgb;
	float Exposure = 0.0;

	switch(u_ToneMapppingType)
	{
		//Reinhardɫ��ӳ��
		case 1:
			HDRColor = HDRColor / (HDRColor + vec3(1.0));
			break;
		//�ع�ɫ��ӳ�䣬�ع��1.0
		case 2:
			Exposure = 1.0;
			HDRColor = vec3(1.0) - exp(-HDRColor * Exposure);
			break;
		//�ع�ɫ��ӳ�䣬�ع��5.0
		case 3:
			Exposure = 5.0;
			HDRColor = vec3(1.0) - exp(-HDRColor * Exposure);
			break;
		//�ع�ɫ��ӳ�䣬�ع��0.1
		case 4:
			Exposure = 0.1;
			HDRColor = vec3(1.0) - exp(-HDRColor * Exposure);
			break;
	}
	//GammaУ��
	HDRColor = pow(HDRColor, vec3(1.0 / 2.2));
	Color_ = vec4(HDRColor, 1.0);
}