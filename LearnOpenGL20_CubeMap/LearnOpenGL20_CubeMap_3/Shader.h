#pragma once
#define GLEW_STATIC
#include <GL/glew.h>  //包含glew来获取OpenGL头文件，否则gl函数都不可用
class shader
{
public:
	//着色器程序对象
	GLuint shaderProgram;
	//构造函数，创建编译顶点着色器、像素着色器，链接着色器程序对象
	shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	//激活当前shader程序对象shaderProgram，供主函数调用
	void Use();
	//从指定路径的文件中读取着色器源码
	const GLchar* GetShaderSourceFromFile(const GLchar* shaderPath);
};