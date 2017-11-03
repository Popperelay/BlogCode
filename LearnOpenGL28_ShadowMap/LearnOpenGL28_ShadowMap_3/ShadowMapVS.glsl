#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 VS_TexCoords;
out vec3 VS_Normal;
out vec3 VS_WorldPos;
out vec4 VS_LightSpacePos;

uniform mat4 u_LightSpaceMVP;  //projection * view * model
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	VS_TexCoords = texCoords;
	VS_Normal = transpose(inverse(mat3(u_Model))) * normal;
	VS_WorldPos = vec3(u_Model * vec4(position, 1.0f));
	gl_Position = u_Projection * u_View * vec4(VS_WorldPos, 1.0f);
	VS_LightSpacePos = u_LightSpaceMVP * vec4(position, 1.0f);
}