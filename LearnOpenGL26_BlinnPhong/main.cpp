#include "shader.h"
#include "camera.h"
#include "TextureLoader.h"
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
const GLuint WIDTH = 800, HEIGHT = 600;
bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat lastCursorX = WIDTH / 2;
GLfloat lastCursorY = HEIGHT / 2;
vec3 lightPos(0.0f, 0.0f, -2.5f);
Camera mycamera;
//��������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffser);
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	//����Ķ������ꡢ��������
	GLfloat planeVertices[] = {
		//λ������            //��������   //��������
		5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,  0, 1, 0,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,  0, 1, 0,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  0, 1, 0,
		5.0f,  -0.5f,  5.0f,  2.0f, 0.0f,  0, 1, 0,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  0, 1, 0,
		5.0f,  -0.5f, -5.0f,  2.0f, 2.0f,  0, 1, 0
	};

	//����planeVAO�����ݽ���
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glBindVertexArray(0);

	shader lightObject("PlaneVS.glsl", "PlaneFS.glsl");
	TextureLoader TexLoader;
	GLuint planeTex = TexLoader.LoadTexture("container.jpg");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//��Ⱦѭ������Ϸѭ����
	while (!glfwWindowShouldClose(window)) {
		//����deltaTime
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		glfwPollEvents(); //����Ƿ����¼����������簴�����¡�����ƶ���
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//�����ܹ�������ɫ���������
		lightObject.Use();
		//ģ�;���
		GLint modelLocation = glGetUniformLocation(lightObject.shaderProgram, "model");
		mat4 model;
		model = translate(model, vec3(-1, 0, -1));
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
		//�۲����
		mat4 view = mycamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
		//ͶӰ����
		mat4 projection = perspective(radians(mycamera.cameraFov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightObject.shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
		//��Դ��ɫ
		glUniform3f(glGetUniformLocation(lightObject.shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
		//��Դλ��
		glUniform3fv(glGetUniformLocation(lightObject.shaderProgram, "lightPos"), 1, &lightPos[0]);
		//�۲�λ��
		glUniform3fv(glGetUniformLocation(lightObject.shaderProgram, "viewPos"), 1, &mycamera.cameraPos[0]);
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, planeTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		do_movement();
		glfwSwapBuffers(window); //������ɫ����
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
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
	if (keys[GLFW_KEY_UP]) {
		mycamera.ProcessMovemnet(mycamera.UP, deltaTime);
	}
	if (keys[GLFW_KEY_DOWN]) {
		mycamera.ProcessMovemnet(mycamera.DOWN, deltaTime);
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