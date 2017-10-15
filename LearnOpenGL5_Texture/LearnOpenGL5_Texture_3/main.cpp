#include "shader.h"
#include "TextureLoader.h"
#include <iostream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
//SOIL
#include <SOIL/SOIL.h>
//���ڳߴ�
const GLuint WIDTH = 800, HEIGHT = 600;
//��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int main() {
	//��ʼ��GLFW
	glfwInit();
	//GLFW����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //�ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ���Ŀ���ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //���ڳߴ粻�ɱ�
											  //�����ܹ�ʹ��GLFW�����Ĵ��ڶ���ָ�룩
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//�������ڻ���
	glfwMakeContextCurrent(window);
	//ע��ص�����
	glfwSetKeyCallback(window, key_callback);
	//��ʼ��GLEW
	glewExperimental = GL_TRUE; //��֤ʹ���ִ���������OpenGL����
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	//������Ⱦ����λ�ô�С
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	//�����ζ�������ꡢ��ɫ����������
	GLfloat vertices[] = {
		//λ������         //��ɫ           //��������
		0.5f, 0.5f, 0.0f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		0.5f, -0.5f,0.0f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,0.0f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f,0.5f, 0.0f,  1.0f,1.0f,0.0f,  0.0f,1.0f
	};
	//��������
	GLuint indices[] = {
		0,1,3,  //��һ��������
		1,2,3   //�ڶ���������
	};
	GLuint VAO, VBO, EBO;
	//���ɶ����������VAO����¼���ݵĴ洢�����ʹ�õ�ϸ����Ϣ
	glGenVertexArrays(1, &VAO);
	//�����㻺�����VBO�����𶥵����ݵĴ洢
	glGenBuffers(1, &VBO);
	//���������������EBO�������������ݵĴ洢
	glGenBuffers(1, &EBO);
	//��������������Ŀ��(�������飬����Ҫ����)
	glBindVertexArray(VAO);
	//�����㻺������Ŀ�꣨�����ڴ棩
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��������������Ŀ�꣨���������ڴ棩
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//�Ѷ������ݸ��Ƶ��Կ��Ļ����ڴ���
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//���������ݸ��Ƶ��Կ��Ļ����ڴ���
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//����OpenGL��ν����Դ��еĶ���λ����������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	//�򿪶���λ����������
	glEnableVertexAttribArray(0);
	//����OpenGL��ν����Դ��еĶ�����ɫ��������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//�򿪶�����ɫ��������
	glEnableVertexAttribArray(1);
	//����OpenGL��ν����Դ��еĶ�������������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//�򿪶�������������������
	glEnableVertexAttribArray(2);
	//���VBO��EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//���VAO
	glBindVertexArray(0);

	TextureLoader textureLoader;
	GLuint texture1 = textureLoader.LoadTexture("container.jpg");
	GLuint texture2 = textureLoader.LoadTexture("awesomeface.png");
	//�����Զ�����ɫ����shader�Ķ���
	shader myshader("TextureVS.glsl", "TextureFS.glsl");
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents(); //����Ƿ����¼����������������¡�����ƶ���
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//������ɫ���������
		myshader.Use();
		//��������Ԫ
		glActiveTexture(GL_TEXTURE0);
		//��ȡuniform����λ��
		GLint uniformLocation1 = glGetUniformLocation(myshader.shaderProgram, "ourTexture1");
		//���ò�������ֵ����Ӧ����Ԫֵ��
		glUniform1i(uniformLocation1, 0);
		//���������
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		GLint uniformLocation2 = glGetUniformLocation(myshader.shaderProgram, "ourTexture2");
		glUniform1i(uniformLocation2, 1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		//��VAO����ɶ��������ʼ��
		glBindVertexArray(VAO);
		//��ͼ
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//���VAO
		glBindVertexArray(0);
		glfwSwapBuffers(window); //������ɫ����
	}
	//ɾ��VAO��VBO��EBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	//����CLFW���ͷ���GLFW�������Դ
	glfwTerminate();
	return 0;
}
//�����ص�����
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}