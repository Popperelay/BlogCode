#pragma once
#define GLEW_STATIC
#include <GL/glew.h>  //����glew����ȡOpenGLͷ�ļ�������gl������������
class shader
{
public:
	//��ɫ���������
	GLuint shaderProgram;
	//���캯�����������붥����ɫ����������ɫ����������ɫ���������
	shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	//���ǰshader�������shaderProgram��������������
	void Use();
	//��ָ��·�����ļ��ж�ȡ��ɫ��Դ��
	const GLchar* GetShaderSourceFromFile(const GLchar* shaderPath);
};