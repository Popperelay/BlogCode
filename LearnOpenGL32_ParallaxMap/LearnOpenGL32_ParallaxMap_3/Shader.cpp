#pragma once
#include "shader.h"
#include <iostream>
using namespace std;
#include <string>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

shader::shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
	//���캯�����������붥����ɫ����������ɫ����������ɫ���������
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

//���ǰshader�������shaderProgram��������������
void shader::Use() {
	glUseProgram(shaderProgram);
}

//��ָ��·�����ļ��ж�ȡ��ɫ��Դ��
const GLchar* shader::GetShaderSourceFromFile(const GLchar* shaderPath) {
	//�Զ����Ʒ�ʽ��ָ��·�����ļ�
	ifstream fin(shaderPath, ios::binary);
	//����Ƿ�ɹ����ļ�
	if (!fin) {
		cout << "Cannot open input file" << endl;
		exit(1);
	}
	//����ָ�����õ��ļ�β
	fin.seekg(0, ios::end);
	//������ǰ�ļ�λ�ã��Ի�ȡ�ļ�����
	int length = fin.tellg();
	//���峤��Ϊlength+1���ַ������洢�ļ�����
	char* shaderSource = new char[length + 1];
	//���½���ָ�����õ��ļ�ͷ
	fin.seekg(0, ios::beg);
	//���������ļ�
	fin.read(shaderSource, length);
	//���ַ���ĩβ�����ַ���������'\0'
	shaderSource[length] = '\0';
	fin.close();
	return shaderSource;
}