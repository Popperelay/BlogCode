#version 330 core
layout (location = 0) in vec3 position;  //顶点位置属性的位置值为0
layout (location = 1) in vec2 texCoords;    //顶点法线属性的位置值为1
layout (location = 2) in vec3 normal;
out vec2 TexCoords;
out vec3 Normal;
out vec3 positionInWorld;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main(){
	gl_Position=projection * view * model * vec4(position,1.0f);
	TexCoords = texCoords;
	Normal=mat3(transpose(inverse(model))) * normal;
	positionInWorld=vec3(model * vec4(position,1.0f));
}