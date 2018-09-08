#version 430 core
layout (location = 0) in vec3 _Position;
layout (location = 1) in vec3 _Normal;
layout (location = 2) in vec2 _TexCoords;

out vec3 v2f_FragWorldPos;
out vec3 v2f_Normal;
out vec2 v2f_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	vec4 FragWorldPos = u_Model * vec4(_Position, 1.0);
	gl_Position = u_Projection * u_View * FragWorldPos;
	v2f_FragWorldPos = vec3(FragWorldPos);
	v2f_Normal =  mat3(transpose(inverse(u_Model))) * _Normal;
	v2f_TexCoords = _TexCoords;
}