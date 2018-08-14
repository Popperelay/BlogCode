#version 430 core

layout(location = 0) in vec3 _Position;
layout(location = 1) in vec3 _Normal;
layout(location = 2) in vec2 _TexCoord;

out vec3 v2f_WorldPos;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(_Position, 1.0);
	v2f_WorldPos = vec3(u_Model * vec4(_Position, 1.0));
}