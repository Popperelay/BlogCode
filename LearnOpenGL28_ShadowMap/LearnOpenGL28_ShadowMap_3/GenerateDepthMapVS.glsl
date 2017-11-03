#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 LightSpaceMVP;  //projection * view * model

void main()
{
	gl_Position = LightSpaceMVP * vec4(position,1.0f);
}