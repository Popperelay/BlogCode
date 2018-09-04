#version 430 core

layout (location = 0) in vec3 _Position;
layout (location = 1) in vec3 _Normal;
layout (location = 2) in vec2 _TexCoord;
layout (location = 3) in vec3 _Tangent;
layout (location = 4) in vec3 _Bitangent;

out vec2 v2f_TexCoord;
out vec3 v2f_FragWorldPos;
out mat3 v2f_TBN;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
	vec4 WorldPosition = u_Model * vec4(_Position, 1.0);
	v2f_FragWorldPos = WorldPosition.xyz;
	
	gl_Position = u_Projection * u_View * WorldPosition;
	v2f_TexCoord = _TexCoord;

	mat3 NormalMatrix = mat3(transpose(inverse(u_Model)));
	vec3 T = normalize(NormalMatrix * _Tangent);
	vec3 B = normalize(NormalMatrix * _Bitangent);
	vec3 N = normalize(NormalMatrix * _Normal);
	v2f_TBN = mat3(T, B, N);
}