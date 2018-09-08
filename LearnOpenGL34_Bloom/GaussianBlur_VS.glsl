#version 430 core
layout (location = 0) in vec3 _Position;
layout (location = 1) in vec2 _TexCoords;

out vec2 v2f_TexCoords;

void main()
{
	gl_Position = vec4(_Position, 1.0);
	v2f_TexCoords = _TexCoords;
}