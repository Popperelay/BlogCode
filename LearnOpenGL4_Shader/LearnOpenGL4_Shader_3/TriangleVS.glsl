//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //位置变量的属性位置值为0
layout (location=1) in vec3 color;     //颜色变量的属性位置值为1
out vec3 ourColor;
void main(){
	gl_Position=vec4(position,1.0f);
	ourColor=color;
}