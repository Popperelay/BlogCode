#include "shader.h"
#include "camera.h"
#include <iostream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
//SOIL
#include <SOIL/SOIL.h>
//GLM
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/glm/gtc/type_ptr.hpp>
using namespace glm;
//���ڳߴ�
const GLuint WIDTH = 800, HEIGHT = 600;
bool keys[1024];
//��ǰ֡����һ֡��ʱ���
GLfloat deltaTime = 0.0f;
//��һ֡��ʼ��ʱ��
GLfloat lastTime = 0.0f;
//��һ֡���λ��
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
//��Դλ��
vec3 lightPos(1.0f, 0.0f, 0.0f);
//�����������Camera�Ķ���
Camera mycamera;
//��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
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
	//ע�ᰴ���ص�����
	glfwSetKeyCallback(window, key_callback);
	//ע�����λ�ûص�����
	glfwSetCursorPosCallback(window, cursor_callback);
	//�������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//ע�������ֻص�����
	glfwSetScrollCallback(window, scroll_callback);
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
	//��������涥������ꡢ����
	GLfloat vertices[] = {
		//λ������           //����  
		//������ǰ��
		0.5f, 0.5f, 0.5f,   0, 0, 1,
		0.5f,-0.5f, 0.5f,   0, 0, 1,
		-0.5f,-0.5f, 0.5f,	 0, 0, 1,
		-0.5f, 0.5f, 0.5f,   0, 0, 1,
		//�������
		0.5f, 0.5f,-0.5f,   0, 0,-1,
		0.5f,-0.5f,-0.5f,   0, 0,-1,
		-0.5f,-0.5f,-0.5f,   0, 0,-1,
		-0.5f, 0.5f,-0.5f,   0, 0,-1,
		//����������
		0.5f, 0.5f, 0.5f,   0, 1, 0,
		0.5f, 0.5f,-0.5f,   0, 1, 0,
		-0.5f, 0.5f,-0.5f,   0, 1, 0,
		-0.5f, 0.5f, 0.5f,   0, 1, 0,
		//����������
		0.5f,-0.5f, 0.5f,   0,-1, 0,
		0.5f,-0.5f,-0.5f,   0,-1, 0,
		-0.5f,-0.5f,-0.5f,   0,-1, 0,
		-0.5f,-0.5f, 0.5f,   0,-1, 0,
		//����������
		0.5f, 0.5f, 0.5f,   1, 0, 0,
		0.5f, 0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f,-0.5f,   1, 0, 0,
		0.5f,-0.5f, 0.5f,   1, 0, 0,
		//����������
		-0.5f, 0.5f, 0.5f,  -1, 0, 0,
		-0.5f, 0.5f,-0.5f,  -1, 0, 0,
		-0.5f,-0.5f,-0.5f,  -1, 0, 0,
		-0.5f,-0.5f, 0.5f,  -1, 0, 0
	};
	//��������
	GLuint indices[] = {
		//������ǰ��
		0,1,3,
		1,2,3,
		//���������
		4,5,7,
		5,6,7,
		//����������
		8,9,11,
		9,10,11,
		//����������
		12,13,15,
		13,14,15,
		//����������
		16,17,19,
		17,18,19,
		//����������
		20,21,23,
		21,22,23
	};
	//��ԴVAO
	GLuint lightSourceVAO, VBO, EBO;
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(lightSourceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	//�ܹ�����VAO
	GLuint lightObjectVAO;
	glGenVertexArrays(1, &lightObjectVAO);
	glBindVertexArray(lightObjectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	//�����Զ�����ɫ����shader�Ķ���
	shader lightSource("LightSourceVS.glsl", "LightSourceFS.glsl");
	shader lightObject("LightingObjectVS.glsl", "LightingObjectFS.glsl");
	glEnable(GL_DEPTH_TEST);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������簴�����¡�����ƶ���
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//�����Դ��ɫ���������
		lightSource.Use();
		//��ȡ3��uniform�����λ��
		GLint modelLocation = glGetUniformLocation(lightSource.shaderProgram, "model");
		GLint viewLocation = glGetUniformLocation(lightSource.shaderProgram, "view");
		GLint projectionLocation = glGetUniformLocation(lightSource.shaderProgram, "projection");
		//�۲����
		mat4 view = mycamera.GetViewMatrix();
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		//ͶӰ����
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		//ģ�;���
		mat4 lightSource_model;
		lightSource_model = translate(lightSource_model, lightPos);
		lightSource_model = rotate(lightSource_model, radians(15.0f), vec3(0.5f, 1.0f, 1.0f));
		lightSource_model = scale(lightSource_model, vec3(0.5f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(lightSource_model));
		//��VAO����ɶ��������ʼ��
		glBindVertexArray(lightSourceVAO);
		//��ͼ
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//���VAO
		glBindVertexArray(0);
		//�����ܹ�������ɫ���������
		lightObject.Use();
		GLint lightColorLoc = glGetUniformLocation(lightObject.shaderProgram, "lightColor");
		GLint objectColorLoc = glGetUniformLocation(lightObject.shaderProgram, "objectColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		modelLocation = glGetUniformLocation(lightObject.shaderProgram, "model");
		viewLocation = glGetUniformLocation(lightObject.shaderProgram, "view");
		projectionLocation = glGetUniformLocation(lightObject.shaderProgram, "projection");
		mat4 lightObject_model;
		lightObject_model = translate(lightObject_model, vec3(-1, 0, -1));
		lightObject_model = rotate(lightObject_model, radians(45.0f), vec3(0.5f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(lightObject_model));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));

		//����uniform������Դλ��lightPos
		GLint lightPosLoc = glGetUniformLocation(lightObject.shaderProgram, "lightPos");
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		//����uniform�����۲�λ��viewPos
		GLint viewPosLoc = glGetUniformLocation(lightObject.shaderProgram, "viewPos");
		glUniform3f(viewPosLoc, mycamera.cameraPos.x, mycamera.cameraPos.y, mycamera.cameraPos.z);
		glBindVertexArray(lightObjectVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		do_movement();
		glfwSwapBuffers(window); //������ɫ����
	}
	//ɾ��VAO��VBO��EBO
	glDeleteVertexArrays(1, &lightSourceVAO);
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
	//��Ӧ���̰�����������Ӧ����״̬
	if (action == GLFW_PRESS) {
		keys[key] = GL_TRUE;
	}
	else if (action == GLFW_RELEASE) {
		keys[key] = GL_FALSE;
	}
}
//���ݰ���״̬�ƶ������
void do_movement() {
	if (keys[GLFW_KEY_W]) {
		mycamera.ProcessMovemnet(mycamera.FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		mycamera.ProcessMovemnet(mycamera.BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		mycamera.ProcessMovemnet(mycamera.RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		mycamera.ProcessMovemnet(mycamera.LEFT, deltaTime);
	}
}
//���λ�ûص�����
bool firstCursor = true;
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstCursor) {
		lastCursorX = xpos;
		lastCursorY = ypos;
		firstCursor = false;
	}
	//����ǰ����֡�����λ����
	GLfloat xOffset = (xpos - lastCursorX);
	GLfloat yOffset = (lastCursorY - ypos);  //ע�����෴�ģ���Ϊ�������ԭ���ڴ������Ͻ�
	lastCursorX = xpos;
	lastCursorY = ypos;
	mycamera.ProcessRotate(xOffset, yOffset);
}
//�����ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mycamera.ProcessScroll(xoffset, yoffset);
}