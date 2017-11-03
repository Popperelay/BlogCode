//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //顶点位置属性的位置值为0
layout (location=2) in vec2 texCoords; //顶点纹理坐标属性的位置值为2
out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position=projection * view * model * vec4(position,1.0f);
	TexCoords=texCoords;
}