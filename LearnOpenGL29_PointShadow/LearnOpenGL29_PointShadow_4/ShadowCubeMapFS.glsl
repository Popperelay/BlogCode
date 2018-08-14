#version 430 core

in vec4 g2f_WorldPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main()
{
	float Light2FragDistance = length(g2f_WorldPos.xyz - u_LightPos) / u_FarPlane;
	gl_FragDepth = Light2FragDistance;
}