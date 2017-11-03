#version 330 core

in vec2 GS_TexCoords;
out vec4 color;
uniform sampler2D diffuse_texture0;

void main(){
	color = texture(diffuse_texture0, GS_TexCoords);
}