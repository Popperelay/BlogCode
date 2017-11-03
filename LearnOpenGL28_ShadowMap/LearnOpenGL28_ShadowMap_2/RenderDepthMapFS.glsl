#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D sampler1;

void main()
{
	float DepthValue = texture(sampler1, TexCoords).r;
	//if(DepthValue > 1.0)
	//	Color = vec4(1,0,0,1);
	//else
	//	Color = vec4(1,1,0,1);
	Color = vec4(vec3(DepthValue), 1.0);
}