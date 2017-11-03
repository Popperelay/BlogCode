#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D sampler1;

void main()
{
	float DepthValue = texture(sampler1, TexCoords).r;
	Color = vec4(vec3(DepthValue), 1.0);
}