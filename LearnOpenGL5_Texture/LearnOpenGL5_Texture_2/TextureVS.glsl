//顶点着色器
#version 330 core
layout (location=0) in vec3 position;  //顶点位置变量的属性位置值为0
layout (location=1) in vec3 color;     //顶点颜色变量的属性位置值为1
layout (location=2) in vec2 texCoord;  //顶点纹理坐标的属性位置值为2
out vec3 ourColor;
out vec2 TexCoord;
void main(){
	gl_Position=vec4(position,1.0f);
	ourColor=color;
	TexCoord=vec2(texCoord.x,1-texCoord.y);
}