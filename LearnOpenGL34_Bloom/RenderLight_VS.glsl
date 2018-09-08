#version 430 core
layout (location = 0) in vec3 _Position;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(_Position, 1.0);
}