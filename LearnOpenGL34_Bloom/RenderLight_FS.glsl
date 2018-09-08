#version 430 core
layout (location = 0) out vec4 FragColor_;
layout (location = 1) out vec4 BrightColor_;

uniform vec3 u_LightColor;

void main()
{
	FragColor_ = vec4(u_LightColor, 1.0);

	float Brightness = dot(FragColor_.rgb, vec3(0.7152, 0.7152, 0.7152));
	if(Brightness > 1.0)
		BrightColor_ = vec4(FragColor_.rgb, 1.0);
	else
		BrightColor_ = vec4(0, 0, 0, 1);
}