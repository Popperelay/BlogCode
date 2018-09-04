#version 430 core

layout (location = 0) in vec3 _Position;
layout (location = 1) in vec2 _TexCoord;
layout (location = 2) in vec3 _Normal;

out vec2 v2f_TexCoord;
out vec3 v2f_FragWorldPos;
out vec3 v2f_Normal;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * vec4(_Position, 1.0);
	v2f_FragWorldPos = _Position;
	v2f_TexCoord = _TexCoord;
	v2f_Normal = _Normal;
}