#pragma once
#include "shader.h"
#include <iostream>
using namespace std;
#include <string>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

shader::shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	//构造函数，创建编译顶点着色器、像素着色器，链接着色器程序对象
	const GLchar* vertexShaderSource = GetShaderSourceFromFile(vertexPath);
	const GLchar* fragmentShaderSource = GetShaderSourceFromFile(fragmentPath);
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	GLint success;
	GLchar infolog[512];
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		cout << "vertex shader compilation failed" << infolog << endl;
	}
	GLuint fragementShader;
	fragementShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragementShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragementShader);
	glGetShaderiv(fragementShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragementShader, 512, NULL, infolog);
		cout << "fragment shader compilation failed" << infolog << endl;
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragementShader);
	if (geometryPath != nullptr) {
		const GLchar* geometryShaderSource = GetShaderSourceFromFile(geometryPath);
		GLuint geometryShader;
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometryShader, 512, NULL, infolog);
			cout << "geometry shader compilation failed" << infolog << endl;
		}
		glAttachShader(shaderProgram, geometryShader);
	}
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		cout << "shader program linking failed" << infolog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragementShader);
}

//激活当前shader程序对象shaderProgram，供主函数调用
void shader::Use() {
	glUseProgram(shaderProgram);
}

//从指定路径的文件中读取着色器源码
const GLchar* shader::GetShaderSourceFromFile(const GLchar* shaderPath) {
	//以二进制方式打开指定路径的文件
	ifstream fin(shaderPath, ios::binary);
	//检测是否成功打开文件
	if (!fin) {
		cout << "Cannot open input file" << endl;
		exit(1);
	}
	//将读指针设置到文件尾
	fin.seekg(0, ios::end);
	//读出当前文件位置，以获取文件长度
	int length = fin.tellg();
	//定义长度为length+1的字符串来存储文件内容
	char* shaderSource = new char[length + 1];
	//重新将读指针设置到文件头
	fin.seekg(0, ios::beg);
	//读出整个文件
	fin.read(shaderSource, length);
	//给字符串末尾加上字符串结束符'\0'
	shaderSource[length] = '\0';
	fin.close();
	return shaderSource;
}