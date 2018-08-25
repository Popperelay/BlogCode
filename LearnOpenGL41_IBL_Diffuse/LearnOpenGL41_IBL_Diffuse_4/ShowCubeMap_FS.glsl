#version 430 core

in vec3 v2f_WorldPos;
out vec4 Color_;

uniform samplerCube u_Cubemap;

void main()
{
	vec3 EnvColor = texture(u_Cubemap, v2f_WorldPos).rgb;
	EnvColor = EnvColor / (EnvColor + vec3(1.0));
    EnvColor = pow(EnvColor, vec3(1.0/2.2)); 

	Color_ = vec4(EnvColor, 1.0);
}