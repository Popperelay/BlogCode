#version 430 core

in vec2 v2f_TexCoord;
out vec4 Color_;

uniform sampler2D u_Texture;

void main()
{
	Color_ = vec4(texture(u_Texture, v2f_TexCoord).rg, 0.0, 1.0);
}