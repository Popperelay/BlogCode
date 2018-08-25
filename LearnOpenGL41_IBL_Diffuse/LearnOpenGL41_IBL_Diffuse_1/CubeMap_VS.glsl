#version 430 core

layout (location=0) in vec3 _Position;

out vec3 v2f_WorldPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	vec4 TempPos = u_Projection * u_View * vec4(_Position,1.0f);
	gl_Position = TempPos.xyzw;
	v2f_WorldPos = _Position;
}