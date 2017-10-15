//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //顶点位置属性的位置值为0
layout (location=1) in vec3 normal;    //顶点法线属性的位置值为1
out vec3 Normal;
out vec3 positionInWorld;
//模型矩阵
uniform mat4 model;
//观察矩阵
uniform mat4 view;
//投影矩阵
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(position,1.0f);
	Normal = mat3(transpose(inverse(model))) * normal;
	positionInWorld = vec3(model * vec4(position, 1.0f));
}