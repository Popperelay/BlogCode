//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //顶点位置变量的属性位置值为0
//模型矩阵
uniform mat4 model;
//观察矩阵
uniform mat4 view;
//投影矩阵
uniform mat4 projection;

void main(){
	gl_Position=projection * view * model * vec4(position,1.0f);
}