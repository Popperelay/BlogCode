#version 430 core

in vec3 v2f_WorldPos;

out vec4 Color_;

uniform samplerCube u_DepthCubeMap;
uniform vec3 u_LightPos;

void main()
{
	vec3 Light2Frag = v2f_WorldPos - u_LightPos;
	float ClosestDepth = texture(u_DepthCubeMap, Light2Frag).r;
	Color_ = vec4(vec3(ClosestDepth), 1.0);
}